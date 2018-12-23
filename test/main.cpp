#include "ninjaskit/ninjaskit.h"

using namespace etk;

void hard_fault_handler(void)
{
    while(true)
    {
    	Serial1.print("Hard fault\r\n");
    	sleep_ms(1000);
    }
}


int main(void)
{
    clock_setup();
    
    Serial1.begin(57600);
    Serial2.begin(57600);
    Serial3.begin(57600);
    SoftSerial.begin();
    
    while(1)
    {
    	SoftSerial.print("Hello world!\r\n");
    	while(SoftSerial.available())
    		Serial1.put(SoftSerial.read());
    	sleep_ms(1);
    }
}


