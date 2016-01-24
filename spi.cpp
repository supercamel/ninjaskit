#include "spi.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/gpio.h>

SPIMasterDriver::SPIMasterDriver(uint32 dr)
{
	spid = dr;
}

void SPIMasterDriver::begin()
{

	switch(spid)
	{
	case SPI1:
	{
		rcc_periph_clock_enable(RCC_SPI1);
		
		gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO5 | GPIO7 );

		gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
				  GPIO6);
	}
	break;
	case SPI2:
	{
		rcc_periph_clock_enable(RCC_SPI2);
		
		gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO13 | GPIO15 );
		gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
				  GPIO14);
	}
	break;
	}
	
	spi_reset(spid);
	
	spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_64, SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE, 
		SPI_CR1_CPHA_CLK_TRANSITION_2, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);

	//spi_set_unidirectional_mode(spid);
	spi_set_receive_only_mode(spid);
	spi_enable_software_slave_management(spid);
	
	spi_disable_crc(spid);
	spi_set_nss_high(spid);
}

void SPIMasterDriver::put(char c)
{
	spi_send(spid, static_cast<uint8_t>(c));
}

char SPIMasterDriver::read()
{
	spi_enable(spid);
	char c = spi_read(spid);
	spi_disable(spid);
	return c;
}

uint8 SPIMasterDriver::transfer(uint8 data)
{
	return spi_xfer(spid, data);
}



SPIMasterDriver spi1(SPI1);
SPIMasterDriver spi2(SPI2);


