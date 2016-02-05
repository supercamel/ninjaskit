#include "flash.h"
#include <libopencm3/stm32/flash.h>
#include "sched.h"

const int FLASH_PAGE_SIZE = 0x800;
const int FLASH_PAGE_NUM_MAX = 127;


uint32 flash_program_data(uint32 start_address, uint8 *input_data, uint16 num_elements)
{
	auto s = scheduler_critical_section();
	
	uint16 iter;
    uint32 current_address = start_address;
    uint32 page_address = start_address;
    uint32 flash_status = 0;

    if((start_address - FLASH_BASE) >= (FLASH_PAGE_SIZE * (FLASH_PAGE_NUM_MAX+1)))
        return 1;

    if(start_address % FLASH_PAGE_SIZE)
        page_address -= (start_address % FLASH_PAGE_SIZE);

    flash_unlock();

    flash_erase_page(page_address);
    flash_status = flash_get_status_flags();
    if(flash_status != FLASH_SR_EOP)
        return flash_status;

    for(iter=0; iter<num_elements; iter += 4)
    {
        flash_program_word(current_address+iter, *((uint32_t*)(input_data + iter)));
        flash_status = flash_get_status_flags();
        if(flash_status != FLASH_SR_EOP)
            return flash_status;

        if(*((uint32_t*)(current_address+iter)) != *((uint32_t*)(input_data + iter)))
            return 0x80;
    }

    return 0;
}


void flash_read_data(uint32 start_address, uint16 num_elements, uint8 *output_data)
{
    uint16 iter;
    uint32 *memory_ptr= (uint32*)start_address;

    for(iter=0; iter<num_elements/4; iter++)
    {
        *(uint32*)output_data = *(memory_ptr + iter);
        output_data += 4;
    }
}

