/*
 * scheduler.h
 * Author: Christian Würthner
 * Description: scheduler for project 2
 */
 
#ifndef __SCHEDULER__
#define __SCHEDULER__

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include "types.h"

/* Pointers to keep track of the processes' current states */
static pcb_t *ready_queue_head = NULL;
static pcb_t *ready_queue_tail = NULL;
static pcb_t *blocked_list_head = NULL;
static pcb_t *blocked_list_tail = NULL;
static pcb_t *running;

/* Used to indicate a new tick. Returns false if the tick was consumed and
   the CPU should not run the process this tick */
bool scheduler_tick();

/* Used to add a new process to the scheduler, start_time will not be checked */
void scheduler_add_process(pcb_t *process);

/* Used to trigger a new scheduling decision */
void scheduler_perform_scheduling();

/* Function called by cpu when a page_fault happend */
void scheduler_trap_page_fault();

/* Function called by cpu when a context switch should be performed */
void scheduler_trap_context_switch();

/* Used to move a process from the ready to the blocked list */
void scheduler_block(pcb_t *process);

/* Used to move a process from the blocked to the ready queue */;
void scheduler_ready(pcb_t *process);

#endif