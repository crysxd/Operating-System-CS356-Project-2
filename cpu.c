/*
 * cpu.c
 * Author: Christian Würthner
 * Description: scheduler for project 2
 */

#include "cpu.h"

/* Define attributes declared as extern in header */
trap cpu_page_fault = NULL;
trap cpu_context_switch = NULL;
trap cpu_timer_done = NULL;
uint32_t cpu_register_mar = 0;
uint32_t cpu_register_mdr = 0;
uint32_t cpu_register_tmr = 0;
instruction_t *cpu_register_ip = NULL;
uint64_t cpu_stall_counter = 0;

/* Used to run a process on the CPU */
void cpu_run(instruction_t *instruction_pointer) {
	cpu_register_ip = instruction_pointer;
}

/* Used to indicate a new tick */
void cpu_tick() {
	/* If no instructions are availabel, the cpu is sattling. Increse counter */
	if(cpu_register_ip == NULL) {
		cpu_stall_counter++;
		printf("[CPU] Stall (%" PRIu64 ")\n", cpu_stall_counter);
		return;
	}

	/* Access memory location */
	printf("[CPU] Accessing %" PRIu32 "\n", cpu_register_ip->address);

	/* Move instruction pointer to next */
	instruction_t *mt = cpu_register_ip;
	cpu_register_ip = cpu_register_ip->next;
	free(mt);

	/* If the next memory location is null, the programm is done */
	if(cpu_register_ip == NULL) {
		cpu_context_switch();
	}

	/* Decrease the timer */
	if(cpu_register_tmr > 0) {
		cpu_register_tmr--;
		printf("[CPU] Timer: %" PRIu32 "\n", cpu_register_tmr);

		/* If the timer is zero, fire the trap */
		if(cpu_register_tmr == 0) {
			printf("[CPU] Timer done!\n");
			cpu_timer_done();
		}
	}
}