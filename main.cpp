#include "ninjaskit/ninjaskit.h"

using namespace etk;


int main(void)
{
	//initialises the clock and peripherals
    clock_setup();
	
	//initialise the serial port using this baud rate
    Serial1.begin(57600);

	while(true)
	{
		//print the message
		Serial1.print("Hello world!\r\n");
		
		//pause for a moment
		sleep_ms(2000);
	}
}








