#ifndef I2C_DRIVER_H_INCLUDED
#define I2C_DRIVER_H_INCLUDED

#include <etk/etk.h>
#include "clock.h"

class I2CMasterDriver
{
public:
	enum I2C_ERROR_TYPE
	{
		I2C_ERROR_START_FAILED,
	};
	
	
    I2CMasterDriver(uint32 dr);
    void begin();
    void end();

    void set_timeout(uint32 millis)
    {
        to_ms = millis;
    }

    bool transmit(uint8 slave, uint8* out, uint32 olen, uint8* in, uint32 ilen);

private:
    uint32 i2cd;
    uint32 to_ms = 10;
};


uint32 i2c_get_last_event(uint32 i2cd);


extern I2CMasterDriver i2c1;
extern I2CMasterDriver i2c2;

#endif

