/*
 * cpu.h
 * Author: Christian Würthner
 * Description: cpu for project 2
 */
 
#ifndef __CPU__
#define __CPU__

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "types.h"
#include "memory.h"
#include "console.h"

/* Function pointer for differnt traps */
extern trap cpu_context_switch;
extern trap cpu_timer_done;

/* Registers */
extern uint32_t cpu_register_tmr; /* Timer Register */
extern instruction_t **cpu_register_ip;  /* Instruction pointer register */

/* Stall counter */
extern uint64_t cpu_stall_counter;

/* Used to indicate a new tick */
void cpu_tick();

/* Used to indicate a tick cannot be used */
void cpu_stall_tick();

/* Used to run a process on the CPU */
void cpu_run(instruction_t **instruction_pointer);

/* Used to preempt the currently running process */
void cpu_preempt();

#endif