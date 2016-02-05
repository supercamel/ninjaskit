/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2015 Jonas Norling <jonas.norling@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
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


template <void (*ENTRY_POINT)(void), uint32 STACK_SZ, uint8 U_ID> auto create_task()
{
	static uint8 __attribute__((aligned(8))) stack[STACK_SZ];
	task_data t;
	t.entry_point = ENTRY_POINT;
	t.stack = stack;
	t.stack_size = STACK_SZ;
	return t;
}

void yield(void);
void scheduler_suspend();
void scheduler_resume();
SCHEDULER_STATUS scheduler_get_status();

template <uint32 N_TASKS> void scheduler_init(const task_data* tt)
{
	void __scheduler_init(task_control_block* tcb, const task_data* tt, uint32 n_tasks);
	
	static struct task_control_block tcbs[N_TASKS];
	__scheduler_init(&tcbs[0], tt, N_TASKS);
}



class scheduler_critical_section
{
public:
	scheduler_critical_section() { scheduler_suspend(); }
	~scheduler_critical_section() { scheduler_resume(); }
};


#endif /* _SCHEDULER_H_ */

