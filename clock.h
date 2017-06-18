#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED

#include <etk/etk.h>

void clock_setup();
etk::Time now();
void sleep_ms(uint32 ms);
void sleep_us(uint32 us);

#endif


