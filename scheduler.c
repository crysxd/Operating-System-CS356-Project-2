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
	if(scheduler_consume_ticks > 0) {
		scheduler_consume_ticks--;
		printf("[SCHEDULER] Performing context switch, %" PRIu64 " ticks left\n"
			, scheduler_consume_ticks);
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
		cpu_preempt();
		printf("[SCHEDULER] Queue is empty!\n");
		return;
	}


	printf("[SCHEDULER] Scheduling, current queue:\n");
	print_list(ready_queue_head, "ready_queue");

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
	cpu_run(&running->instruction_pointer);

	printf("[SCHEDULER] Put process \"%s\" on CPU\n", running->name);

	/* Move the beginning of the beginning of the queue to the next element */
	ready_queue_head = ready_queue_head->next;

	/* Consume the next 50 ticks to simulate context switching time */
	scheduler_consume_ticks = 5;
}

/* Function called by cpu when a page_fault happend */
void scheduler_trap_page_fault() {
	printf("[SCHEDULER] Page fault for page %d\n", 
		running->instruction_pointer->address);

	/* Call pager to load the needed page, will block the running process and
	   so cause resheduling */
	pager_load(running);

}

/* Function called by cpu when a context switch should be performed */
void scheduler_trap_context_switch() {
	printf("[SCHEDULER] Trap context switch running...\n");

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

/* Used to move the currently running process from the ready to 
   the blocked list */
void scheduler_block() {
	printf("[SCHEDULER] \"%s\" is blocked\n", 
		running->name);

	/* Append the process to the list of blocked processes or set as head if
	   list is empty */
	if(blocked_list_head == NULL) {
		blocked_list_head = running;
	} else {
		blocked_list_tail->next = running;
	}

	/* Set process as new tail */
	blocked_list_tail = running;
	blocked_list_tail->next = NULL;

	/* Perform a context switch to a new process */
	scheduler_perform_scheduling();
}

/* Used to move a process from the blocked to the ready queue */;
void scheduler_ready(pcb_t *process) {
	printf("[SCHEDULER] \"%s\" is runnable\n", 
		process->name);

	/* If process is the head of the blocked list, replace the head */
	if(blocked_list_head == process) {
		blocked_list_head = blocked_list_head->next;
	}

	/* Else search for the process and cut it out */
	else {
		/* Iterate over blocked list */
		pcb_t *e = blocked_list_head;
		while(e != NULL) {
			/* If the next element after e is process */
			if(e->next == process) {
				/* Cut process out of queue */
				e->next = process->next;
				process->next = NULL;
				break;
			}

			e = e->next;
		}
	}

	/* Add process to ready queue */
	scheduler_move_process_to_ready_queue_tail(process);

	/* Trigger scheduling if no process is rsunning */
	if(running == NULL) {
		scheduler_perform_scheduling();
	}
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