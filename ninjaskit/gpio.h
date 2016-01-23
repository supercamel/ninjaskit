#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include <etk/etk.h>

enum : uint8 {
	PA,
	PB,
	PC,
	PD
} typedef GPIO_BANK;


struct gpio_pin
{
	gpio_pin() { }
	
	gpio_pin(GPIO_BANK bank, int pin) : bank(bank), pin(pin)
	{
	
	}
	
	GPIO_BANK bank;
	int pin;
};


enum : uint8 {
	PULL_UP,
	PULL_DOWN,
	OPEN_DRAIN
} typedef PIN_PULL_DIRECTION;

const bool HIGH = true;
const bool LOW = false;

void set_pin(gpio_pin pin, bool high);
void toggle_pin(gpio_pin pin);
bool read_pin(gpio_pin pin);

void configure_as_input(gpio_pin pin, PIN_PULL_DIRECTION pull = OPEN_DRAIN);
void configure_as_output(gpio_pin pin);


#endif


