/*
 * scheduler.h
 * Author: Christian Würthner
 * Description: scheduler for project 2
 */
 
#ifndef __SCHEDULER__
#define __SCHEDULER__

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include "types.h"
#include "cpu.h"
#include "pager.h"

/* The quantum used by rounf robin */
extern uint32_t scheduler_rr_quantum;

/* Pointers to keep track of the processes' current states */
extern pcb_t *ready_queue_head;
extern pcb_t *ready_queue_tail;
extern pcb_t *blocked_list_head;
extern pcb_t *blocked_list_tail;
extern pcb_t *scheduler_running;

/* Couter to use ticks for context switch */
extern uint64_t scheduler_consume_ticks;

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
void scheduler_block();

/* Used to move a process from the blocked to the ready queue */;
void scheduler_ready(pcb_t *process);

/* Used to append a process to the tail of the ready queue. If the process given
   is the current head, it will be moved to the tail */
void scheduler_move_process_to_ready_queue_tail(pcb_t *process);

/* Prints all elements of the list */
void print_list(pcb_t *list_head, char *list_name); 

/* Checks if there are un finished processes */
bool scheduler_all_processes_done();

#endif