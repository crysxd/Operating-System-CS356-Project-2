/*
 * scheduler.c
 * Author: Christian Würthner
 * Description: scheduler for project 2
 */

#include "scheduler.h"

/* Define attributes declared as extern in header */
uint32_t scheduler_rr_quantum = 200;

/* Used to indicate a new tick. Returns false if the tick was consumed and
   the CPU should not run the process this tick */
bool scheduler_tick() {
	if(consume_ticks > 0) {
		consume_ticks--;
		printf("[SCHEDULER] Performing context switch, %" PRIu64 " ticks left\n"
			, consume_ticks);
		return false;
	}

	return true;
}

/* Used to add a new process to the scheduler, start_time will not be checked */
void scheduler_add_process(pcb_t *process) {
	/* Move process to the end of the ready queue */
	scheduler_move_process_to_ready_queue_tail(process);

	/* Print status */
	printf("[SCHEDULER] Added new process \"%s\"\n", process->name);

	/* Start scheduling if there is no process running */
	if(running == NULL) {
		scheduler_perform_scheduling();
	}
}


/* Used to trigger a new scheduling decision */
void scheduler_perform_scheduling() {
	if(ready_queue_head == NULL) {
		running = NULL;
		printf("[SCHEDULER] Queue is empty!\n");
		return;
	}

	/* Reset CPU timer to guarantee that the control is given back to the 
	   scheduler after the time quantum is elapsed */
	cpu_register_tmr = scheduler_rr_quantum;

	if(running == ready_queue_head) {
		printf("[SCHEDULER] Process \"%s\" is allowed to stay on CPU!\n", 
			running->name);
		return;
	}

	/* Put process on CPU and save to running */
	running = ready_queue_head;
	cpu_run(running->instruction_pointer);

	printf("[SCHEDULER] Put process \"%s\" on CPU\n", running->name);
	print_list(ready_queue_head, "ready_queue");

	/* Move the beginning of the beginning of the queue to the next element */
	ready_queue_head = ready_queue_head->next;

	/* Consume the next 50 ticks to simulate context switching time */
	consume_ticks = 50;
}

/* Function called by cpu when a page_fault happend */
void scheduler_trap_page_fault() {

}

/* Function called by cpu when a context switch should be performed */
void scheduler_trap_context_switch() {
	printf("[SCHEDULER] Trap context switch running...\n");
	running->instruction_pointer = cpu_register_ip;

	/* If the process is not finished yet, put it back to the ready queue. Else
	   free its memory and delete it */
	if(running->instruction_pointer != NULL) {
		scheduler_move_process_to_ready_queue_tail(running);
	} else {
		printf("[SCHEDULER] Process \"%s\" is done.\n", running->name);
		free(running->name);
		free(running);
	}

	/* Trigger scheduling */
	scheduler_perform_scheduling();
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
	/* If the process is the current head, fix the queue */
	if(ready_queue_head == process) {
		ready_queue_head = process->next;
	}

	/* Link process to previous in queue or set as head if queue is empty */
	if(ready_queue_head == NULL) {
		ready_queue_head = process;
	} else {
		ready_queue_tail->next = process;
	}

    /* Set process as new tail in queue, set next to NULL */
	ready_queue_tail = process;
	process->next = NULL;
}

/* Prints all elements of the list */
void print_list(pcb_t *list_head, char *list_name) {
	printf("%s:\n", list_name);
	printf("-----------------------------------------------------\n");
	while(list_head != NULL) {
		printf("| %-10s | %-10" PRIu64 " | %-10" PRIu64 " | %-10" PRIu64 " |\n", 
			list_head->name, list_head->start_time, 
			list_head->cpu_time, list_head->io_count);

		list_head = list_head->next;
	}
	printf("-----------------------------------------------------\n");
}

/* Checks if there are un finished processes */
bool scheduler_all_processes_done() {
	return ready_queue_head == NULL && blocked_list_head == NULL && 
	running == NULL;
}