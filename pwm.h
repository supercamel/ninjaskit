#ifndef PWM_H_INCLUDED
#define PWM_H_INCLUDED

#include <etk/etk.h>
#include "gpio.h"


/**
 * Ninjask uses two hardware timers to generate pulses on up to four digital pins each.
 *
 * Each Pulse class has four channels, and each channel is connected to a digital pin. For Pulse1, these pins are
 *		PA0, PA1, PA2, PA3
 *
 * For Pulse2 these pin are
 *		PA6, PA7, PB0, PB1
 *
 */
class PulseWidth
{
public:
    PulseWidth(uint32 timer);

    void begin(uint16 period = 20000);

    void set_period(uint16 p);
    void enable_output(uint8 channel);
    void set_pulse_width(uint8 channel, uint16 width);
    void start_timer();

private:
    uint32 timer_peripheral = 0;
};

extern PulseWidth Pulse1;
extern PulseWidth Pulse2;


#endif

