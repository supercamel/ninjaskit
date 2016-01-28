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
	//initialises the clock and peripherals
    clock_setup();
    
	//initialise the serial port using this baud rate
    Serial1.begin(57600);
    
    sleep_ms(3000);
    
    Serial1.print("Starting. . . \r\n");

	auto inp = gpio_pin({PA, 5});
	configure_as_input(inp);
	
	Pulse2.begin();
	
	Pulse2.enable_output(0);
    Pulse2.enable_output(1);
    Pulse2.enable_output(2);
    Pulse2.enable_output(3);
    
    Pulse2.set_pulse_width(0, 1500);
    Pulse2.set_pulse_width(1, 1500);
    Pulse2.set_pulse_width(2, 1500);
    Pulse2.set_pulse_width(3, 1500);
    
    Pulse2.start_timer();
	while(true)
	{
		//print the message
		Serial1.print(pulse_in(inp), "us\r\n");
		
		//pause for a moment
		sleep_ms(200);
	}
}








