#ifndef USART_DRIVER_H_INCLUDED
#define USART_DRIVER_H_INCLUDED

#include <etk/etk.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>


const uint16 USART_RING_BUFFER_SIZE = 64;


class UsartDriver : public etk::Stream<UsartDriver>
{
public:
    UsartDriver(uint32 usartn);

    void begin(uint32 baud, uint32 bits=8, uint32 stopbits=USART_STOPBITS_1, uint32 parity=USART_PARITY_NONE);
    void disable();

	bool is_full() volatile
	{
		return (end + 1) % USART_RING_BUFFER_SIZE == start;
	}
	
    uint32 available() volatile
    {
        return (USART_RING_BUFFER_SIZE + end - start) % USART_RING_BUFFER_SIZE;
    }

    void put(char c)
    {
        usart_send_blocking(usart, c);
    }

    void println(auto& buf)
    {
        print(buf);
        print("\r\n");
    }

    void write(auto& buf, uint32 len)
    {
        for(uint32 i = 0; i < len; i++)
            usart_send_blocking(usart, buf[i]);
    }

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
    

    friend void usart1_isr(void);
    friend void usart2_isr(void);
    friend void usart3_isr(void);

};


extern UsartDriver Serial1;
extern UsartDriver Serial2;
extern UsartDriver Serial3;

#endif

