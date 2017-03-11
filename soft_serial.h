#ifndef SOFT_SERIAL_H_INCLUDED
#define SOFT_SERIAL_H_INCLUDED

#include <etk/etk.h>
#include "gpio.h"

class SoftwareSerial
{
public:
	SoftwareSerial(gpio_pin tx, gpio_pin rx);
	void begin(uint32 baud);
	
};


#endif

