/*
 * cpu.h
 * Author: Christian Würthner
 * Description: cpu for project 2
 */
 
#ifndef __CPU__
#define __CPU__

#include <stdio.h>
#include <inttypes.h>
#include "types.h"

/* Function pointer for differnt traps */
trap cpu_page_fault;
trap cpu_context_switch;

/* Registers */
uint32_t cpu_register_mar; /* Memory Address Register */
uint32_t cpu_register_mdr; /* Memory Data Register */

/* Used to indicate a new tick */
void cpu_tick();

/* Used to run a process on the CPU */
void cpu_run(memory_trace_t *process, uint64_t *instruction_pointer);

/* Used to preempt the currently running process */
void cpu_preempt();

#endif