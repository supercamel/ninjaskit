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

void green_led_task(void)
{
	while(true)
	{
		Serial1.print("red task: ", etk::now().seconds(), "\r\n");
		etk::sleep_ms(100);
	}
}

void blue_led_task(void)
{
	while(true)
	{
		if(read_pin({PB, 1}))
			scheduler_suspend();
		else
			scheduler_resume();
		
		etk::sleep_ms(250);
	}
}


int main(void)
{
    clock_setup();
    
    Serial1.begin(57600);
    
    configure_as_output({PA, 5});
    configure_as_input({PB, 1});
    
    const uint32 n_tasks = 2;
    const struct task_data task_table[n_tasks] = {
		create_task<green_led_task, 256>(),
		create_task<blue_led_task, 256>()};
	
    scheduler_init<n_tasks>(&task_table[0]);

    while(1);
}


