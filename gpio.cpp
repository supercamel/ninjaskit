#include "gpio.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>


static uint32 gpio_n_from_bank(GPIO_BANK bank)
{
    switch(bank)
    {
    case PA:
        return GPIOA;
    case PB:
        return GPIOB;
    case PC:
        return GPIOC;
    case PD:
        return GPIOD;
    }

    return 0;
}

void set_pin(gpio_pin pin, bool high)
{
    if(high)
        gpio_set(gpio_n_from_bank(pin.bank), (1 << pin.pin));
    else
        gpio_clear(gpio_n_from_bank(pin.bank), (1 << pin.pin));
}


void toggle_pin(gpio_pin pin)
{
    gpio_toggle(gpio_n_from_bank(pin.bank), (1 << pin.pin));
}


bool read_pin(gpio_pin pin)
{
    return gpio_get(gpio_n_from_bank(pin.bank), (1 << pin.pin));
}

void configure_as_input(gpio_pin pin, PIN_INPUT_TYPE ipt)
{
    uint32 gpio = gpio_n_from_bank(pin.bank);

    switch(ipt)
    {
    case INPUT_FLOAT:
        gpio_set_mode(gpio, GPIO_MODE_INPUT,
                      GPIO_CNF_INPUT_FLOAT, (1 << pin.pin));
        break;
    case INPUT_ANALOG:
        gpio_set_mode(gpio, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, (1 << pin.pin));
        break;
    }

}

uint32 pulse_in(gpio_pin pin)
{
    while(read_pin(pin) == LOW) { }

    auto start = etk::now();

    while(read_pin(pin) == HIGH) { }

    return etk::now().diff_time(start)*1000000;
}


void configure_as_output(gpio_pin pin)
{
    uint32 gpio = gpio_n_from_bank(pin.bank);

    gpio_set_mode(gpio, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, (1 << pin.pin));
}



