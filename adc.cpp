#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/adc.h>
#include "adc.h"
#include "sched.h"

void adc_setup()
{
    rcc_periph_clock_enable(RCC_ADC1);

    /* Make sure the ADC doesn't run during config. */
    adc_off(ADC1);

    /* We configure everything for one single conversion. */
    adc_disable_scan_mode(ADC1);
    adc_set_single_conversion_mode(ADC1);
    adc_disable_external_trigger_regular(ADC1);
    adc_set_right_aligned(ADC1);
    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_28DOT5CYC);

    adc_power_on(ADC1);

    /* Wait for ADC starting up. */
    int i;
    for (i = 0; i < 800000; i++) /* Wait a bit. */
        __asm__("nop");

    /* Select the channel we want to convert. 16=temperature_sensor. */
    uint8_t channel_array[16];
    channel_array[0] = 0;
    adc_set_regular_sequence(ADC1, 1, channel_array);

    /* Start the conversion directly (not trigger mode). */
    adc_start_conversion_direct(ADC1);
}

uint16_t adc_read(uint8_t ch)
{
	auto s = scheduler_critical_section();
	
    while (!(ADC_SR(ADC1) & ADC_SR_EOC));
    uint16_t last_res = ADC_DR(ADC1);

    /* Select the channel we want to convert. 16=temperature_sensor. */
    uint8_t channel_array[16];
    channel_array[0] = ch;
    adc_set_regular_sequence(ADC1, 1, channel_array);

    /* Start the conversion directly (not trigger mode). */
    adc_start_conversion_direct(ADC1);
    
    return last_res;
}

