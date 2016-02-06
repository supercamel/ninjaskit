/*
 * This scheduler code is mostly based upon work by
 *
 * Copyright (C) 2015 Jonas Norling <jonas.norling@gmail.com>
 *
 * https://github.com/JonasNorling/libopencm3-examples/tree/master/examples/stm32/f1/stm32vl-discovery/multithreading
 *
 * C++ port/adaption by Samuel Cowen <samuel.cowen@camelsoftware.com>
 *
 */

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "sched.h"
#include "usart.h"


struct stack_frame 
{
	struct 
	{
		uint32_t registers[8];
	} software_frame;

	struct 
	{
		uint32_t r0;
		uint32_t r1;
		uint32_t r2;
		uint32_t r3;
		uint32_t r12;
		void *lr;
		void *pc;
		uint32_t psr;
	} nvic_frame;
};


static task_control_block* tcbs = nullptr;
static uint8_t current_task;
static uint32 n_tasks = 0;
static task_data* task_table = nullptr;
static SCHEDULER_STATUS status = SCHEDULER_INVALID;


void return_from_task(void)
{
	while (true);
}

void __scheduler_init(task_control_block* tcb, const task_data* tt, uint32 nt)
{
	tcbs = tcb;
	task_table = const_cast<task_data*>(tt);
	n_tasks = nt;
	uint32 i;
	for (i = 0; i < n_tasks; i++) 
	{
		const struct task_data *task = &task_table[i];

		tcbs[i].sp = (uint8_t *)task->stack + task->stack_size;

		tcbs[i].sp -= sizeof(struct stack_frame);
		struct stack_frame *frame = (struct stack_frame *)tcbs[i].sp;
		frame->nvic_frame.r0 = 0xff00ff00;
		frame->nvic_frame.r1 = 0xff00ff01;
		frame->nvic_frame.r2 = 0xff00ff02;
		frame->nvic_frame.r3 = 0xff00ff03;
		frame->nvic_frame.r12 = 0xff00ff0c;
		frame->nvic_frame.lr = (void*)return_from_task;
		frame->nvic_frame.pc = (void*)task->entry_point;
		frame->nvic_frame.psr = 0x21000000; /* Default, allegedly */
		frame->software_frame.registers[0] = 0xff00ff04;
		frame->software_frame.registers[1] = 0xff00ff05;
		frame->software_frame.registers[2] = 0xff00ff06;
		frame->software_frame.registers[3] = 0xff00ff07;
		frame->software_frame.registers[4] = 0xff00ff08;
		frame->software_frame.registers[5] = 0xff00ff09;
		frame->software_frame.registers[6] = 0xff00ff0a;
		frame->software_frame.registers[7] = 0xff00ff0b;

		tcbs[i].runnable = true;
	}
	
	nvic_enable_irq(NVIC_TIM4_IRQ);
	nvic_set_priority(NVIC_TIM4_IRQ, 1);
	
	rcc_periph_clock_enable(RCC_TIM4);
	TIM_CNT(TIM4) = 1;
	TIM_PSC(TIM4) = 64;
	TIM_ARR(TIM4) = 1000;
	TIM_DIER(TIM4) |= TIM_DIER_UIE;
	TIM_CR1(TIM4) |= TIM_CR1_CEN;
	status = SCHEDULER_RUNNING;
}


static void scheduler_switch(void)
{
	do 
	{
		current_task = (current_task + 1) % n_tasks;
	} while (!tcbs[current_task].runnable);
}



void __attribute__((naked)) pend_sv_handler(void)
{
	if(status != SCHEDULER_RUNNING)
		return;
	
	const uint32_t RETURN_ON_PSP = 0xfffffffd;
	
	uint32_t lr;
	__asm__("MOV %0, lr" : "=r" (lr));

	if (lr & 0x4) 
	{
		void *psp;
		__asm__(
			"MRS %0, psp\n"
			"STMDB %0!, {r4-r11}\n"
			"MSR psp, %0\n"
			: "=r" (psp));

		tcbs[current_task].sp = psp;
	} 
	else
		__asm__("STMDB SP!, {r4-r11}");

	scheduler_switch();

	void *psp = tcbs[current_task].sp;
	__asm__(
		"LDMFD %0!, {r4-r11}\n"
		"MSR psp, %0\n"
		: : "r" (psp));

	__asm__("bx %0" : : "r"(RETURN_ON_PSP));
}

void yield(void)
{
	SCB_ICSR |= SCB_ICSR_PENDSVSET;
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
}

void scheduler_suspend()
{
	if(status != SCHEDULER_INVALID)
		status = SCHEDULER_SUSPENDED;
}

void scheduler_resume()
{
	if(status == SCHEDULER_SUSPENDED)
		status = SCHEDULER_RUNNING;
}

SCHEDULER_STATUS scheduler_get_status()
{
	return status;
}


void tim4_isr(void)
{
	TIM_SR(TIM4) &= ~TIM_SR_UIF; //clear interrupt flag
	if(status == SCHEDULER_RUNNING)
		yield();
}



