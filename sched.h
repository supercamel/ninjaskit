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

#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <etk/etk.h>

enum SCHEDULER_STATUS : uint8
{
	SCHEDULER_RUNNING,
	SCHEDULER_SUSPENDED,
	SCHEDULER_INVALID
};

//halts the current thread and moves to the next one
void yield(void);

//suspends the scheduler - context switching won't occur until scheduler_resume() is called
void scheduler_suspend();
//resumes context switching
void scheduler_resume();
//returns the state of the scheduler
SCHEDULER_STATUS scheduler_get_status();

//uses constructor/destructor is automagically suspend and resume the scheduler during critical paths
class scheduler_critical_section
{
public:
	scheduler_critical_section() { scheduler_suspend(); }
	~scheduler_critical_section() { scheduler_resume(); }
};


struct task_data
{
	void (*entry_point)(void) = nullptr;
	void *stack = nullptr;
	unsigned stack_size;
};

struct task_control_block
{
	void *sp;
	bool runnable;
};

/*
 * This metaprogrammy-type trick creates stack space for each thread in BSS/global space.
 *
 * By passing different template parameters, the compile is forced to generate multiple different 'create_task' functions.
 * Each one of these generated functions has it's own static array ( 'stack' ) which is put in global space. 
 * The stack size is specified by the STACK_SZ parameter, and the function to run by ENTRY_POINT.
 *
 * create_task() then returns a copy of a new task_data struct complete with a unique stack pointer.
 */
template <void (*ENTRY_POINT)(void), uint32 STACK_SZ> auto create_task()
{
	static uint8 __attribute__((aligned(8))) stack[STACK_SZ];
	task_data t;
	t.entry_point = ENTRY_POINT;
	t.stack = stack;
	t.stack_size = STACK_SZ;
	return t;
}

/*
 * scheduler_init creates a task_control_block and initialises all the stuff.
 *
 * It starts hardware timer TIM4, which triggers interrupts at a 1Khz rate. 
 * These interrupts are used to preempt the current thread and trigger context switches.
 *
 */
template <uint32 N_TASKS> void scheduler_init(const task_data* tt)
{
	void __scheduler_init(task_control_block* tcb, const task_data* tt, uint32 n_tasks);
	
	static struct task_control_block tcbs[N_TASKS];
	__scheduler_init(&tcbs[0], tt, N_TASKS);
}



#endif /* _SCHEDULER_H_ */

