#ifndef I2C_DRIVER_H_INCLUDED
#define I2C_DRIVER_H_INCLUDED

#include <etk/etk.h>


class I2CMasterDriver
{
public:
    I2CMasterDriver(uint32 dr);
    void begin();
    void end();

    void set_timeout(uint32 millis) {
        to_ms = millis;
    }
    bool transmit(uint8 slave, uint8* out, uint32 olen, uint8* in, uint32 ilen);

private:
    uint32 i2cd;
    uint32 to_ms;
};


uint32 i2c_get_last_event(uint32 i2cd);


extern I2CMasterDriver twi;
extern I2CMasterDriver twi1;

#endif

