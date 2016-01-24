#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

#include <etk/etk.h>

void adc_setup();
uint16 adc_read(uint8 channel);

#endif

