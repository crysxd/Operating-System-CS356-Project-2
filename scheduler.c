/*
 * scheduler.c
 * Author: Christian Würthner
 * Description: scheduler for project 2
 */

#include "scheduler.h"

/* Define attributes declared as extern in header */
uint32_t scheduler_rr_quantum = 200;
pcb_t *scheduler_running = NULL;

/* Used to indicate a new tick. Returns false if the tick was consumed and
   the CPU should not run the process this tick */
bool scheduler_tick() {
	if(scheduler_consume_ticks > 0) {
		scheduler_consume_ticks--;
		console_log("SCHEDULER", 
			"Performing context switch, %" PRIu64 " ticks left", 
			scheduler_consume_ticks);
		return false;
	}

	/* If there is something to execute for the CPU, increase cpu time */
	if(cpu_register_ip != NULL) {
		scheduler_running->cpu_time++;
	}

	return true;
}

/* Used to add a new process to the scheduler, start_time will not be checked */
void scheduler_add_process(pcb_t *process) {
	/* Move process to the end of the ready queue */
	scheduler_move_process_to_ready_queue_tail(process);

	/* Print status */
	console_log("SCHEDULER", "Added new process \"%s\"", process->name);

	/* Start scheduling if there is no process running */
	if(scheduler_running == NULL) {
		scheduler_perform_scheduling();
	}
}


/* Used to trigger a new scheduling decision */
void scheduler_perform_scheduling() {
	/* This is a hot fix for a segmentation fault happening if an empty
	   process is in the ready queue. Empty processes are skipped */
	while(ready_queue_head != NULL && ready_queue_head->instruction_pointer == 0) {
		console_error("SCHEDULER", "Found empty program in ready queue");
		console_error("SCHEDULER", "Restoring from error state");
		console_error("SCHEDULER", "Freeing and skipping %s", ready_queue_head->name);
		ready_queue_head = ready_queue_head->next;
	}

	/* If the ready queue is empty, skip this scheduling time */
	if(ready_queue_head == NULL) {
		scheduler_running = NULL;
		cpu_preempt();
		console_log("SCHEDULER", "Queue is empty!");
		return;
	}

	console_log("SCHEDULER", "Scheduling, current queue:");
	print_list(ready_queue_head, "ready_queue");

	/* Reset CPU timer to guarantee that the control is given back to the 
	   scheduler after the time quantum is elapsed */
	cpu_register_tmr = scheduler_rr_quantum;

	/* If the process is allowed to stay one the cpu, 
	   no context switch is needed */
	if(scheduler_running == ready_queue_head) {
		console_log("SCHEDULER", "Process \"%s\" is allowed to stay on CPU!", 
			scheduler_running->name);
		return;
	}
	

	/* Put process on CPU and save to running */
	scheduler_running = ready_queue_head;
	cpu_run(&scheduler_running->instruction_pointer);

	console_log("SCHEDULER", "Put process \"%s\" on CPU", 
		scheduler_running->name);

	/* Move the beginning of the beginning of the queue to the next element */
	ready_queue_head = ready_queue_head->next;

	/* Assure there is no loop */
	if(ready_queue_head != NULL && ready_queue_head->next == ready_queue_head) {
		console_error("SCHEDULER", "Loop in ready queue detected");
		exit(0);
	}

	/* Consume the next 50 ticks to simulate context switching time */
	scheduler_consume_ticks = SCHEDULER_CONTEXT_SWITCH_TIME;
}

/* Function called by cpu when a page_fault happend */
void scheduler_trap_page_fault() {
	console_log("SCHEDULER", "Page fault for page %d", 
		scheduler_running->instruction_pointer->address);

	/* Increase page fault counter */
	scheduler_running->page_faults++;

	/* Call pager to load the needed page, will block the running process and
	   so cause resheduling */
	pager_load(scheduler_running);

}

/* Function called by cpu when a context switch should be performed */
void scheduler_trap_context_switch() {
	console_log("SCHEDULER", "Trap context switch running...");

	/* If the process is not finished yet, put it back to the ready queue. Else
	   free its memory and delete it */
	if(scheduler_running->instruction_pointer != NULL) {
		scheduler_move_process_to_ready_queue_tail(scheduler_running);

	} else {
		console_log("SCHEDULER", "Process \"%s\" is done.", 
			scheduler_running->name);

		/* Print results for process */
		uint64_t turnaround_time = cpu_time - scheduler_running->start_time;
		uint64_t waiting_time = turnaround_time - scheduler_running->cpu_time;
		console_log_force("RESULT", "%-20s %s (pid: %" PRIu32 ")", 
			"Name:", scheduler_running->name, scheduler_running->pid);
		console_log_force("RESULT", "%-20s %" PRIu64, 
			"Arrival time:", scheduler_running->start_time);
		console_log_force("RESULT", "%-20s %" PRIu64, 
			"Turnaround time:", turnaround_time);
		console_log_force("RESULT", "%-20s %" PRIu64, 
			"Waiting time:", waiting_time);
		console_log_force("RESULT", "%-20s %" PRIu64, 
			"CPU time:", scheduler_running->cpu_time);
		console_log_force("RESULT", "%-20s %" PRIu64, 
			"Page faults:", scheduler_running->page_faults);
		console_log_force("RESULT", 
			"================================================================" 
			"===============");

		/* Free data */
		console_log("SCHEDULER", "Free data of %s", scheduler_running->name);
		free(scheduler_running->name);
		free(scheduler_running);
		scheduler_running = NULL;
	}

	/* Trigger scheduling */
	scheduler_perform_scheduling();
}

/* Used to move the currently running process from the ready to 
   the blocked list */
void scheduler_block() {
	console_log("SCHEDULER", "\"%s\" is blocked", scheduler_running->name);

	/* Append the process to the list of blocked processes or set as head if
	   list is empty */
	if(blocked_list_head == NULL) {
		blocked_list_head = scheduler_running;
	} else {
		blocked_list_tail->next = scheduler_running;
	}

	/* Set process as new tail */
	blocked_list_tail = scheduler_running;
	blocked_list_tail->next = NULL;

	/* Perform a context switch to a new process */
	scheduler_perform_scheduling();
}

/* Used to move a process from the blocked to the ready queue */;
void scheduler_ready(pcb_t *process) {
	console_log("SCHEDULER", "\"%s\" is runnable", process->name);

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
	if(scheduler_running == NULL) {
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
	console_log("SCHEDULER", "%s:", list_name);
	console_log("SCHEDULER", 
		"-------------------------------------------------------------------------------");
	console_log("SCHEDULER",
		"| %-10s | %-10s | %-10s | %-10s | %-10s | %-10s |",
		"PID", "Name", "Start Time", "CPU Time", "Wait Time", "Page Fault");
	console_log("SCHEDULER", 
		"-------------------------------------------------------------------------------");
	while(list_head != NULL) {
		console_log("SCHEDULER", 
			"| %-10" PRIu32 " | %-10s | %-10" PRIu64 " | %-10" PRIu64 " "
			"| %-10" PRIu64 " | %-10" PRIu64 " |", 
			list_head->pid,
			list_head->name, 
			list_head->start_time,
			list_head->cpu_time,
			list_head->waiting_time,
			list_head->page_faults);

		list_head = list_head->next;
	}
	console_log("SCHEDULER", 
		"-------------------------------------------------------------------------------");
}

/* Checks if there are un finished processes */
bool scheduler_all_processes_done() {
	return ready_queue_head == NULL && blocked_list_head == NULL && 
	scheduler_running == NULL;
}