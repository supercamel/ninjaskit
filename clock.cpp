#include "clock.h"
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <etk/etk.h>


void sys_tick_handler(void)
{
    etk::tick();
}

void clock_setup()
{
    rcc_clock_setup_in_hsi_out_64mhz();
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);

	rcc_periph_clock_enable(RCC_AFIO);
	
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);

    //64Mhz, AHB_DIV8, 64,000,000/8 = 8,000,000
    //we're after a 5us clock which means one tick every 200,000 microseconds
    //8,000,000/200,000 = 40
    //40-1 = 39
    systick_set_reload(7999);
    //systick_set_reload(1000);
    systick_interrupt_enable();
    systick_counter_enable();

    etk::set_tick_rate(1000);
}



