/*
 * scheduler.c
 * Author: Christian Würthner
 * Description: scheduler for project 2
 */

#include "scheduler.h"


/* Used to indicate a new tick. Returns false if the tick was consumed and
   the CPU should not run the process this tick */
bool scheduler_tick() {
	return true;
}

/* Used to add a new process to the scheduler, start_time will not be checked */
void scheduler_add_process(pcb_t *process) {

}

/* Used to trigger a new scheduling decision */
void scheduler_perform_scheduling() {

}

/* Function called by cpu when a page_fault happend */
void scheduler_trap_page_fault() {

}

/* Function called by cpu when a context switch should be performed */
void scheduler_trap_context_switch() {

}

/* Used to move a process from the ready to the blocked list */
void scheduler_block(pcb_t *process) {

}

/* Used to move a process from the blocked to the ready queue */;
void scheduler_ready(pcb_t *process) {

}

/* Used to append a process to the tail of the ready queue. If the process given
   is the current head, it will be moved to the tail */
void scheduler_move_process_to_ready_queue_tail(pcb_t *process) {
	
}