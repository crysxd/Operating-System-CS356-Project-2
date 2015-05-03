/*
 * control.c
 * Author: Christian Würthner
 * Description: control for project 2
 */

#include "control.h"

int main(int argc, char **argv) {
	/* Check argument count */
	if(argc < 3) {
		printf("Not enough arguments, usage: "
			"simulator PATH_TO_PROCESS_TRACE PATH_TO_MEMORY_TRACES\n");
		return ERR_NO_TOO_FEW_ARGS;
	}

	/* Set traps for cpu */
	cpu_page_fault = &scheduler_trap_page_fault;
	cpu_context_switch = &scheduler_trap_context_switch;
	cpu_timer_done = &scheduler_trap_context_switch;

	/* Read process trace */
	control_create_process_arrival_queue(argv[1]);

	print_list(control_process_queue_head, "arrival_list");

	/* Perform ticks */
	while(control_process_queue_head != NULL || !scheduler_all_processes_done()) {
		printf("\n=============================\n"
			"[CONTROL] CPU time: %" PRIu64 "\n", cpu_time);
		/* Check if a new process arrives to the current time */
		if(control_process_queue_head != NULL && 
			control_process_queue_head->start_time == cpu_time) {
			/* Add it and remove it from the arrival queue */
			pcb_t *p = control_process_queue_head;
			control_process_queue_head = p->next;
			control_load_memory_trace(p, argv[2]);
			scheduler_add_process(p);
		}

		/* Perform a scheduler tick, if the tick is not used perform a cpu 
		   tick */
		if(scheduler_tick()) {
			cpu_tick();
		}

		/* Increase CPU time */
		cpu_time++;
		usleep(10000);
	}
}

/* Loads the process file and puts the proceeses into 
   control_process_queue_head */
void control_create_process_arrival_queue(char *trace_file_name) {
	/* Open file, handle errors */
	FILE* trace_file;
	if((trace_file = fopen(trace_file_name, "r")) == NULL) {
		printf("ERROR: Unable to read process trace file \"%s\" (%s)",
			trace_file_name, strerror(errno));
		exit(ERR_NO_UNABLE_OPEN_PROCESS_TRACE);
	}

	/* Iterate over all lines */
	char line[1024];
	int32_t line_result = 0;

	for(;;) {
		/* Read current line */
		line_result = read_line(trace_file, line);

		/* If the line is empty, we are at the end of the file, break */
		if(line_result == 0) {
			break;
		}

		/* If a negative value is returned, a error occured */
		if(line_result < 0) {
			printf("ERROR: Unable to read memory trace file \"%s\" (%s)",
				trace_file_name, strerror(errno));
			exit(ERR_NO_UNABLE_OPEN_MEMORY_TRACE);
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
    	pcb->io_count = atof(values[3]);
    	pcb->end_time = -1;
    	pcb->instruction_pointer = NULL;
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

/* Loads the memory trace file for the given process from the given directory */
void control_load_memory_trace(pcb_t *process, char *dir) {
	/* Define needed variables */
	char line[1024];
	int32_t line_result = 0;
	instruction_t *last = NULL;

	/* Generate the full path */
	char full_path[PATH_MAX];
	sprintf(full_path, "%s/%s", dir, process->name);

	/* Open file */
	FILE* trace_file;
	if((trace_file = fopen(full_path, "r")) == NULL) {
		printf("ERROR: Unable to read memory trace file \"%s\" (%s)",
			full_path, strerror(errno));
		exit(ERR_NO_UNABLE_OPEN_MEMORY_TRACE);
	}

	/* Iterate over all lines */
	for(;;) {
		/* Read current line */
		line_result = read_line(trace_file, line);

		/* If the line is empty, we are at the end of the file, break */
		if(line_result == 0) {
			break;
		}

		/* If a negative value is returned, a error occured */
		if(line_result < 0) {
			printf("ERROR: Unable to read memory trace file \"%s\" (%s)",
				full_path, strerror(errno));
			exit(ERR_NO_UNABLE_OPEN_MEMORY_TRACE);
		}

		/* Read number and append to memory_trace list */
		instruction_t *t = malloc(sizeof(instruction_t));
		t->address = atof(line);

		/* Append to list or staet list */
		if(process->instruction_pointer == NULL) {
			process->instruction_pointer = t;
		} else {
			last->next = t;
		}

		/* Set t to last */
		last = t;
		last->next = NULL;
	}
}

/* Reads a single line from the file pointer */
int32_t read_line(FILE *file, char *dest) {
	uint32_t line_position = 0;
	size_t read_count = 0;

	for(;;) {
		/* Read single byte */
		read_count = fread(dest+line_position, 1, 1, file);

		/* Check for error or file end or line end */
		if(read_count <= 0 || dest[line_position] == '\n') {
			/* Error -> return error */
			if(ferror(file)) {
				return -1;
			} else {
				/* line or file end, override with string end and break */
				break;
			}
		}

		line_position++;
	}

	/* Set last char in line to 0 to indicate string end */
	dest[line_position] = 0;
	return line_position;

}