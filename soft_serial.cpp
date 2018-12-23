#include "soft_serial.h"
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/stm32/f1/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/cm3/cortex.h>


static volatile bool byte_cleared = true;
static volatile uint8_t tx_byte = 0;
static volatile uint8_t tx_bit = 0;

static volatile uint8_t rx_byte = 0;
static volatile uint8_t rx_bit = 0;

static gpio_pin tx;
static gpio_pin rx;


SoftwareSerial SoftSerial({PB, 0}, {PB, 1});

SoftwareSerial::SoftwareSerial(gpio_pin t, gpio_pin r)
{
	tx = t;
	rx = r;
}

void SoftwareSerial::begin()
{
	configure_as_output(tx);
	configure_as_input(rx);
	set_pin(tx, HIGH);
	
	
	rcc_periph_clock_enable(RCC_TIM2);
	timer_reset(TIM2);
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
						TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_prescaler(TIM2, 124);
	timer_set_repetition_counter(TIM2, 0);
    timer_enable_preload(TIM2);
    timer_continuous_mode(TIM2);
    timer_set_period(TIM2, 4);
	timer_disable_preload(TIM2);
	timer_enable_counter(TIM2);
	nvic_enable_irq(NVIC_TIM2_IRQ);
	timer_enable_irq(TIM2, TIM_DIER_CC1IE);
	
	
	rcc_periph_clock_enable(RCC_TIM3);
	timer_reset(TIM3);
	timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT,
						TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_prescaler(TIM3, 4);
	timer_set_repetition_counter(TIM3, 0);
	timer_enable_preload(TIM3);
	timer_continuous_mode(TIM3);
	timer_set_period(TIM3, 124);
	timer_disable_preload(TIM3);
	nvic_enable_irq(NVIC_TIM3_IRQ);
	timer_enable_irq(TIM3, TIM_DIER_CC1IE);
	
	
    nvic_enable_irq(NVIC_EXTI1_IRQ);
    exti_select_source((1 << 1), GPIOB);
    exti_set_trigger((1 << 1), EXTI_TRIGGER_FALLING);
    exti_enable_request((1 << 1));
}

void SoftwareSerial::put(uint8_t b)
{
	while(byte_cleared == false) { }
	tx_byte = b;
	tx_bit = 0;
	byte_cleared = false;
	
	while(byte_cleared == false) { }
}


extern "C" void tim2_isr()
{
    if (timer_get_flag(TIM2, TIM_SR_CC1IF))
    {
		timer_clear_flag(TIM2, TIM_SR_CC1IF);

		if(byte_cleared == false)
		{
			if(tx_bit == 0)
			{
				set_pin(tx, LOW);
				tx_bit++;
			}
			else if(tx_bit == 9)
			{
				set_pin(tx, HIGH);
				byte_cleared = true;
			}
			else
			{
				set_pin(tx, (tx_byte & (1 << (tx_bit-1))));
				tx_bit++;
			}
		}
	}
}

extern "C" void tim3_isr()
{
	cm_disable_interrupts();
	auto pin_state = read_pin(rx);
	
	if (timer_get_flag(TIM3, TIM_SR_CC1IF))
    {
    	timer_clear_flag(TIM3, TIM_SR_CC1IF);
    	timer_set_period(TIM3, 124);
    	
		rx_byte |= (pin_state << rx_bit);
		rx_bit++;
		if(rx_bit >= 8)
		{
			timer_disable_counter(TIM3);
			exti_enable_request(1 << 1);
			SoftSerial.push(rx_byte);
		}
	}
	cm_enable_interrupts();
}


void exti1_isr(void)
{
	exti_reset_request((1 << 1));
	exti_disable_request((1 << 1));
	
	timer_set_period(TIM3, 164);
	
	rx_byte = 0;
	rx_bit = 0;
	timer_enable_counter(TIM3);
}




