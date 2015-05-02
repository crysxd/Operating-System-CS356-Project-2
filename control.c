/*
 * control.c
 * Author: Christian Würthner
 * Description: control for project 2
 */

#include "control.h"

int main(int argc, char **argv) {
	/* Check argument count */
	if(argc < 2) {
		printf("Not enough arguments, usage: "
			"simulator PATH_TO_PROCESS_TRACE\n");
		return ERR_NO_TOO_FEW_ARGS;
	}

	/* Set traps for cpu */
	cpu_page_fault = &scheduler_trap_page_fault;
	cpu_context_switch = &scheduler_trap_context_switch;

	/* Read process trace */
	control_create_process_arrival_queue(argv[1]);

	print_list(control_process_queue_head, "arrival_list");

	/* Perform ticks */
	while(control_process_queue_head != NULL || !scheduler_all_processes_done()) {

		/* Check if a new process arrives to the current time */
		if(control_process_queue_head != NULL && 
			control_process_queue_head->start_time == cpu_time) {
			/* Add it and remove it from the arrival queue */
			pcb_t *p = control_process_queue_head;
			control_process_queue_head = p->next;
			scheduler_add_process(p);
		}

		/* Increase CPU time */
		cpu_time++;
	}
}

void control_create_process_arrival_queue(char *trace_file_name) {
	/* Open file, handle errors */
	FILE* trace_file;
	if((trace_file = fopen(trace_file_name, "r")) == NULL) {
		printf("ERROR: Unable to read prcess trace file \"%s\" (%s)",
			trace_file_name, strerror(errno));
		exit(ERR_NO_UNABLE_OPEN_PROCESS_TRACE);
	}

	/* Iterate over all lines */
	char line[1024];
	uint16_t counter = 0;
	size_t read_count = 0;

	for(;;) {
		/* Read current line */
		counter = 0;
		for(;;) {
			/* Read single byte */
			read_count = fread(line+counter, 1, 1, trace_file);

			/* Check for error or file end or line end */
			if(read_count <= 0 || line[counter] == '\n') {
				/* Error -> Print error message and exit */
				if(ferror(trace_file)) {
					printf("ERROR: error while reading trace file! (%s)\n",
						strerror(errno));
					exit(ERR_NO_PROCESS_TRACE_PARSE);
				} else {
					/* line or file end, override with string end and break */
					line[counter] = 0;
					break;
				}
			}

			counter++;
		}

		/* If the line is empty, we are at the end of the file, break */
		if(strlen(line) == 0) {
			break;
		}

		/* Split the line at tab to get the single values */
		char values[4][512];
		char *p;
    	for(uint8_t i=0; i<4; i++) {
    		/* Get part */
    		p = strtok(i==0 ? line : NULL, "\t ");

    		/* Handle error */
    		if(p == NULL) {
    			printf("ERROR: error while parsing trace file! (%s)\n",
						strerror(errno));
    			exit(ERR_NO_PROCESS_TRACE_PARSE);
    		}

    		/* copy */
    		int length = strlen(p);
    		memcpy(values+i, p, length);
    		values[i][length] = 0;
    	}

    	/* Create new pcb and fill in information */
    	pcb_t *pcb = malloc(sizeof(pcb_t));
    	pcb->name = malloc(strlen(values[0]));
    	memcpy(pcb->name, values, strlen(values[0])+1);
    	pcb->cpu_time = SECS_TO_TICKS(atof(values[2]));
    	pcb->start_time = SECS_TO_TICKS(atof(values[1]));
    	pcb->waiting_time = 0;
    	pcb->instruction_pointer = 0;
    	pcb->io_count = atof(values[3]);
    	pcb->end_time = -1;
    	pcb->memory_trace = NULL;
    	pcb->next = NULL;

    	/* Print result */
    	printf("SUCESS: created process control block for \"%s\"\n", pcb->name);
    	printf("\tstart_time = %" PRIu64 "\n", pcb->start_time);
		printf("\tcpu_time = %" PRIu64 "\n", pcb->cpu_time);
		printf("\tio_count = %" PRIu64 "\n", pcb->io_count);

    	/* Link pcb to previous in queue or set as head if queue is empty */
    	if(control_process_queue_head == NULL) {
    		control_process_queue_head = pcb;
    	} else {
    		control_process_queue_tail->next = pcb;
    	}

    	/* Set pcb as new tail in queue */
    	control_process_queue_tail = pcb;
	}
}
