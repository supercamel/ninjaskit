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
    
    configure_as_input({PB, 1}, INPUT_PULLUP);
    
    while(1)
    {
    	Serial1.print(read_pin({PB, 1}), "\r\n");
    	sleep_ms(100);
    }
}


