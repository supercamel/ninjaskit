#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

#include <stdint.h>

void adc_setup();
uint16_t adc_read(uint8_t channel);

#endif

