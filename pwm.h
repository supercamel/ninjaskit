#ifndef PWM_H_INCLUDED
#define PWM_H_INCLUDED

#include <etk/etk.h>
#include "gpio.h"


class PulseWidth
{
public:
    PulseWidth(uint32 timer);

    void begin(uint32 period = 20000);

    void set_period(uint32 p);
    void enable_output(uint8 channel);
    void set_pulse_width(uint8 channel, uint32 width);
    void start_timer();

private:
    uint32 timer_peripheral = 0;
};

extern PulseWidth Pulse1;
extern PulseWidth Pulse2;


#endif

