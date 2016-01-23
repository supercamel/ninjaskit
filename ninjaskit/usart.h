#ifndef USART_DRIVER_H_INCLUDED
#define USART_DRIVER_H_INCLUDED

#include <etk/etk.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>


const uint16 USART_RING_BUFFER_SIZE = 255;


class UsartDriver : public etk::Stream<UsartDriver>
{
public:
    UsartDriver(uint32 usartn);

    void begin(uint32 baud, uint32 bits=8, uint32 stopbits=USART_STOPBITS_1, uint32 parity=USART_PARITY_NONE);
    void end();

    uint32 available()
    {
        return ringbuf.available();
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

    uint8 read()
    {
        if(ringbuf.available())
            return ringbuf.get();
        return 0;
    }

    uint8 peek_ahead(uint16 n)
    {
        return ringbuf.peek_ahead(n);
    }

    void empty() {
        ringbuf.empty();
    }

private:
    uint32 usart;
    uint8 buffer[USART_RING_BUFFER_SIZE];
    etk::RingBuffer<uint8> ringbuf;

    friend void usart1_isr(void);
    friend void usart2_isr(void);
    friend void usart3_isr(void);

};


extern UsartDriver Serial1;
extern UsartDriver Serial2;
extern UsartDriver Serial3;

#endif

