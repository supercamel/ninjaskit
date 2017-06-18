#include "clock.h"
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <etk/etk.h>
#include "sched.h"

using namespace etk;

static volatile Time _now;

void sys_tick_handler(void)
{
    _now.micros() += 10;

	if(_now.micros() >= 1000000)
	{
		_now.seconds()++;
		_now.micros() = 0;
	}
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
    systick_set_reload(79);
    //systick_set_reload(1000);
    systick_interrupt_enable();
    systick_counter_enable();
}

/**
 * \brief Returns the current time.
 */
Time __attribute__((weak)) now()
{
	return _now;
}

/**
	 * \brief Sleeps for a number of milliseconds.
	 * @arg ms Number of milliseconds to wait.
	 */
void __attribute__((weak)) sleep_ms(uint32 ms)
{
    Time start = now();
	real_t sms = ms/1000.0f;

    while(now().diff_time(start) < sms)
    { }
}

/**
 * \brief Sleeps for a number of microseconds.
 * @args us Number of microseconds to wait.
 */
void __attribute__((weak)) sleep_us(uint32 us)
{
    Time start = now();
	real_t sus = us/1000000.0f;

    while(now().diff_time(start) < sus)
    { }
}


