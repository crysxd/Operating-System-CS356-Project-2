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

	/* Set traps for cpu */
	cpu_page_fault = &scheduler_trap_page_fault;
	cpu_context_switch = &scheduler_context_switch;

}
