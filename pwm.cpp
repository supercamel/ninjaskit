#include "pwm.h"

#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>

PulseWidth::PulseWidth(uint32 timer)
{
    timer_peripheral = timer;
}

void PulseWidth::begin(uint16 period, bool microsecond_precision)
{
    if(timer_peripheral == TIM2)
        rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM2EN);
    else if(timer_peripheral == TIM3)
        rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM3EN);
    else
        return;

    timer_reset(timer_peripheral);

    /* Set the timers global mode to:
     * - use no divider
     * - alignment edge
     * - count direction up
     */
    timer_set_mode(timer_peripheral,
                   TIM_CR1_CKD_CK_INT,
                   TIM_CR1_CMS_EDGE,
                   TIM_CR1_DIR_UP);

	if(microsecond_precision)
    	timer_set_prescaler(timer_peripheral, 64);
    else
    	timer_set_prescaler(timer_peripheral, 64'000);
    
    timer_set_repetition_counter(timer_peripheral, 0);
    timer_enable_preload(timer_peripheral);
    timer_continuous_mode(timer_peripheral);
    timer_set_period(timer_peripheral, period);
}

void PulseWidth::set_period(uint16 period)
{
    timer_set_period(timer_peripheral, period);
}

void PulseWidth::enable_output(uint8 channel)
{
    if(timer_peripheral == TIM2)
    {
        if(channel >= 4)
            return;

        //rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN);

        gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                      (1 << channel));
    }
    else if(timer_peripheral == TIM3)
    {
        switch(channel)
        {
        case 0:
        case 1:
        {
            //rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
            gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                          GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                          (1 << (channel+6)));
        }
        break;
        case 2:
        case 3:
        {
            //rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPBEN);
            gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                          GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                          (1 << (channel-2)));
        }
        break;
        }
    }
    else
        return;

    tim_oc_id tid = static_cast<tim_oc_id>((channel%4)*2);

    timer_disable_oc_output(timer_peripheral, tid);
    timer_set_oc_mode(timer_peripheral, tid, TIM_OCM_PWM1);
    timer_set_oc_value(timer_peripheral, tid, 0);
    timer_enable_oc_output(timer_peripheral, tid);
}

void PulseWidth::set_pulse_width(uint8 channel, uint16 width)
{
    tim_oc_id tid = static_cast<tim_oc_id>((channel%4)*2);
    timer_set_oc_value(timer_peripheral, tid, width);
}

void PulseWidth::start_timer()
{
    timer_enable_counter(timer_peripheral);
}


PulseWidth Pulse1(TIM2);
PulseWidth Pulse2(TIM3);

