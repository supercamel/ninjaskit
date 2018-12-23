#ifndef SOFT_SERIAL_H_INCLUDED
#define SOFT_SERIAL_H_INCLUDED

#include <etk/etk.h>
#include "gpio.h"
#include "usart.h"

class SoftwareSerial : public etk::Stream<SoftwareSerial>
{
public:
	SoftwareSerial(gpio_pin tx, gpio_pin rx);
	void begin();
	
	bool is_full() volatile
	{
		return (end + 1) % USART_RING_BUFFER_SIZE == start;
	}
	
    uint32 available() volatile
    {
        return (USART_RING_BUFFER_SIZE + end - start) % USART_RING_BUFFER_SIZE;
    }
    
    
	void put(uint8_t b);
	
	uint8 read() volatile
    {
        if(available())
        {
            uint8 ret = buffer[start];
        	start = (start + 1) % USART_RING_BUFFER_SIZE;
        	return ret;
        }
        return 0;
    }

    uint8 peek_ahead(uint16 n) volatile
    {
        uint16 pos = (start+n) % USART_RING_BUFFER_SIZE;
        return buffer[pos];
    }

    void empty() volatile
    {
        start = 0;
        end = 0;
    }



private:
	uint32 usart;
    volatile uint8 buffer[USART_RING_BUFFER_SIZE];
    uint32 start = 0;
    volatile uint32 end = 0;
    
    void push(uint8 b) volatile
    {
        if(is_full())
            return;
            
        buffer[end] = b;
        end = (end + 1) % USART_RING_BUFFER_SIZE;
    }
    

    friend void tim3_isr();

};

extern SoftwareSerial SoftSerial;

#endif

