#include "i2c.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/cm3/cortex.h>


using namespace etk;


I2CMasterDriver::I2CMasterDriver(uint32 dr)
{
    i2cd = dr;
}

void I2CMasterDriver::begin()
{
    /* Disable the I2C before changing any configuration. */
    i2c_peripheral_disable(i2cd);

    i2c_reset(i2cd);

    switch(i2cd)
    {
    case I2C1:
    {
        rcc_periph_clock_enable(RCC_I2C1);
        gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                      GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
                      GPIO_I2C1_SCL | GPIO_I2C1_SDA);
    }
    break;
    case I2C2:
    {
        rcc_periph_clock_enable(RCC_I2C2);

        gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                      GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
                      GPIO_I2C2_SCL | GPIO_I2C2_SDA);
    }
    break;
    }

    /* APB1 is running at 36MHz. */
    i2c_set_clock_frequency(i2cd, I2C_CR2_FREQ_32MHZ);
    i2c_set_standard_mode(i2cd);
    /*
    * fclock for I2C is 36MHz APB2 -> cycle time 28ns, low time at 400kHz
    * incl trise -> Thigh = 1600ns; CCR = tlow/tcycle = 0x1C,9;
    * Datasheet suggests 0x1e.
    */
    i2c_set_ccr(i2cd, 0x1c);
    /*
    * fclock for I2C is 36MHz -> cycle time 28ns, rise time for
    * 400kHz => 300ns and 100kHz => 1000ns; 300ns/28ns = 10;
    * 1000ns/28ns = 36; 36+1=37=0x25
    * Incremented by 1 -> 11.
    */
    i2c_set_trise(i2cd, 0x23);
    /*
    * This is our slave address - needed only if we want to receive from
    * other masters.
    */
    i2c_set_own_7bit_slave_address(i2cd, 0x32);

    i2c_enable_interrupt(i2cd, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);

    /* If everything is configured -> enable the peripheral. */
    i2c_peripheral_enable(i2cd);

    i2c_enable_ack(i2cd);

    /*
    	nvic_enable_irq(NVIC_I2C2_EV_IRQ); //TODO enabling interrupts here causes it to hang
    			nvic_enable_irq(NVIC_I2C2_ER_IRQ);

    			nvic_set_priority(NVIC_I2C2_EV_IRQ, 0);
    */
}


void I2CMasterDriver::end()
{
    i2c_peripheral_disable(i2cd);
}


bool I2CMasterDriver::transmit(uint8 slave, uint8* out, uint32 olen, uint8* in, uint32 ilen)
{
    Time msnow = now();

    uint32 reg32 __attribute__((unused));

    I2C_CR1(i2cd) |= I2C_CR1_ACK;

    /* Send START condition. */
    i2c_send_start(i2cd);

    /* Waiting for START is send and therefore switched to master mode. */
    while (!((I2C_SR1(i2cd) & I2C_SR1_SB)
             & (I2C_SR2(i2cd) & (I2C_SR2_MSL | I2C_SR2_BUSY))))
    {
        if(now().diff_time_ms(msnow) > to_ms)
            return false;
    }

    /* Say to what address we want to talk to. */
    i2c_send_7bit_address(i2cd, slave, I2C_WRITE);

    /* Waiting for address is transferred. */
    while (!(I2C_SR1(i2cd) & I2C_SR1_ADDR))
    {
        if(now().diff_time_ms(msnow) > to_ms)
            return false;
    }

    /* Cleaning ADDR condition sequence. */
    reg32 = I2C_SR2(i2cd);

    for(uint32 i = 0; i < olen; i++)
    {
        i2c_send_data(i2cd, out[i]); /* */
        while (!(I2C_SR1(i2cd) & I2C_SR1_BTF))
        {
            if(now().diff_time_ms(msnow) > to_ms)
                return false;
        }
    }

    while (!(I2C_SR1(i2cd) & (I2C_SR1_BTF | I2C_SR1_TxE)))
    {
        if(now().diff_time_ms(msnow) > to_ms)
            return false;
    }


    if((in != 0) && (ilen > 0))
    {
        i2c_send_start(i2cd);

        /* Waiting for START is send and switched to master mode. */
        while (!((I2C_SR1(i2cd) & I2C_SR1_SB)
                 & (I2C_SR2(i2cd) & (I2C_SR2_MSL | I2C_SR2_BUSY))))
        {
            if(now().diff_time_ms(msnow) > to_ms)
                return false;
        }

        /* Say to what address we want to talk to. */
        i2c_send_7bit_address(i2cd, slave, I2C_READ);

        /* Receiving from I2C using polling
        	AN2824 shows three methods for 1, 2 and multi-byte receives.
        */
        switch(ilen)
        {
        case 1:
        {
            I2C_CR1(i2cd) |= (I2C_CR1_POS | I2C_CR1_ACK);

            /* Waiting for address is transferred. */
            while (!(I2C_SR1(i2cd) & I2C_SR1_ADDR))
            {
                if(now().diff_time_ms(msnow) > to_ms)
                    return false;
            }

            //clear ACK bit
            I2C_CR1(i2cd) &= ~I2C_CR1_ACK;

            cm_disable_interrupts();
            //clear ADDR
            reg32 = I2C_SR2(i2cd);
            //generate stop
            i2c_send_stop(i2cd);
            cm_enable_interrupts();

            //wait until rx not empty = 1
            while (!(I2C_SR1(i2cd) & (I2C_SR1_RxNE)))
            {
                if(now().diff_time_ms(msnow) > to_ms)
                    return false;
            }

            //read the byte
            in[0] = i2c_get_data(i2cd);

        }
        break;
        case 2:
        {
            I2C_CR1(i2cd) |= (I2C_CR1_POS | I2C_CR1_ACK);

            /* Waiting for address is transferred. */
            while (!(I2C_SR1(i2cd) & I2C_SR1_ADDR))
            {
                if(now().diff_time_ms(msnow) > to_ms)
                    return false;
            }

            cm_disable_interrupts();
            //clear ADDR
            reg32 = I2C_SR2(i2cd);
            //clear ACK bit
            I2C_CR1(i2cd) &= ~I2C_CR1_ACK;
            cm_enable_interrupts();

            //wait for BTF
            while (!(I2C_SR1(i2cd) & I2C_SR1_BTF))
            {
                if(now().diff_time_ms(msnow) > to_ms)
                    return false;
            }

            cm_disable_interrupts();
            in[0] = i2c_get_data(i2cd);
            //generate stop
            I2C_CR1(i2cd) |= I2C_CR1_STOP;
            cm_enable_interrupts();

            in[1] = i2c_get_data(i2cd);

        }
        break;
        default:
        {
            /* Waiting for address is transferred. */
            while (!(I2C_SR1(i2cd) & I2C_SR1_ADDR))
            {
                if(now().diff_time_ms(msnow) > to_ms)
                    return false;
            }
            //clear ADDR
            reg32 = I2C_SR2(i2cd);

            while(ilen-- != 3)
            {
                //wait for BTF
                while (!(I2C_SR1(i2cd) & I2C_SR1_RxNE))
                {
                    if(now().diff_time_ms(msnow) > to_ms)
                        return false;
                }
                //receive data
                *in++ = i2c_get_data(i2cd);
            }

            //wait for BTF
            while (!(I2C_SR1(i2cd) & (I2C_SR1_RxNE)))
            {
                if(now().diff_time_ms(msnow) > to_ms)
                    return false;
            }

            cm_disable_interrupts();
            //read data
            *in++ = i2c_get_data(i2cd);
            //set stop bit
            cm_enable_interrupts();

            while (!(I2C_SR1(i2cd) & (I2C_SR1_RxNE)))
            {
                if(now().diff_time_ms(msnow) > to_ms)
                    return false;
            }

            *in++ = i2c_get_data(i2cd);

            i2c_disable_ack(i2cd);
            i2c_nack_current(i2cd);

            //wait until rx not empty = 1
            while (!(I2C_SR1(i2cd) & (I2C_SR1_RxNE)))
            {
                if(now().diff_time_ms(msnow) > to_ms)
                    return false;
            }

            //read data
            *in++ = i2c_get_data(i2cd);

            i2c_send_stop(i2cd);
        }

        }
    }
    else
        i2c_send_stop(i2cd);

    i2c_enable_ack(i2cd);
    I2C_CR1(i2cd) &= ~I2C_CR1_POS;

    return true;
}

I2CMasterDriver twi(I2C1);
I2CMasterDriver twi1(I2C2);


