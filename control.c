/*
 * control.c
 * Author: Christian Würthner
 * Description: control for project 2
 */

#include <stdio.h>
#include "scheduler.h"
#include "cpu.h"
#include "memory.h"
#include "pager.h"

int main(int argc, char **argv) {
	/* Check argument count */
	if(argc < 2) {
		printf("Not enough arguments, usage: "
			"simulator PATH_TO_PROCESS_TRACE\n");
		return 1;
	}

	/* Set traps for cpu */
	cpu_page_fault = &scheduler_trap_page_fault;
	cpu_context_switch = &scheduler_trap_context_switch;

}
