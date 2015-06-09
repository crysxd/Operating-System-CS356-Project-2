/*
 * control.c
 * Author: Christian Würthner
 * Description: control for project 2
 */

#include "control.h"

int main(int argc, char **argv) {
	/* Check argument count */
	if(argc < 5) {
		console_error("CONTROL", 
			"Not enough arguments, usage: "
			"mem‐sim pages quantum pr‐policy trace‐file [verbose [step]]");
		return ERR_NO_TOO_FEW_ARGS;
	}

	/* Parse all args */
	memory_frame_count = atoi(argv[1]);
	scheduler_rr_quantum = atoi(argv[2]);
	char *trace_file_name = argv[4];

	/* Using strcmp to create a unique value for the page replacement algo */
	switch(strcmp("fifo", argv[3])) {
		case 0:  pager_page_replacement_algo = 0;  break;
		case -6: pager_page_replacement_algo = -6; break;
		case 52: pager_page_replacement_algo = 52; break;
		default: console_error("CONTROL", 
			"%s is not a valid page replacement algorithm", argv[3]);
				return ERR_NO_UNDEFINED_PAGE_REPLACEMENT_ALGORITHM;
	}

	/* If the 6. parameter is verbose, set verbose mode to true */
	if(argc > 5 && strcmp(argv[5], "verbose") == 0) {
		console_verbose_mode = true;
		console_log("CONTORL", "Verbose mode is enabled");
	}

	/* If the 7. parameter is step, enable step mode */
	if(argc > 6 && strcmp(argv[6], "step") == 0) {
		control_step_mode = true;
		console_log("CONTORL", "Step mode is enabled");
	}

	/* Set traps for cpu */
	memory_page_fault = &scheduler_trap_page_fault;
	cpu_context_switch = &scheduler_trap_context_switch;
	cpu_timer_done = &scheduler_trap_context_switch;

	/* Read process trace */
	control_create_process_arrival_queue(trace_file_name);
	print_list(process_queue_head, "arrival_list");

	/* Prepare memory */
	memory_init();

	uint64_t skip_step_beaks = 0;

	/* Perform ticks [MAIN LOOP] */
	while(process_queue_head != NULL || !scheduler_all_processes_done()) {
		console_log("CONTROL", 
			"================================================================" 
			"===============");
		console_log("CONTROL", "CPU time: %" PRIu64, cpu_time);

		/* Check if a new process arrives to the current time */
		if(process_queue_head != NULL && 
			process_queue_head->start_time == cpu_time) {
			/* Add it and remove it from the arrival queue */
			pcb_t *p = process_queue_head;
			process_queue_head = p->next;
			control_load_memory_trace(p, "");
			scheduler_add_process(p);
		}

		/* Perform a pager tick */
		pager_tick();

		/* Perform a scheduler tick, if the tick is not used perform a cpu 
		   tick, otherwise inform the cpu about the wasted tick */
		if(scheduler_tick()) {
			cpu_tick();
		} else {
			cpu_stall_tick();
		}

		/* Increase CPU time */
		cpu_time++;

		/* If step mode is enabled wait for confirm of user */
		if(control_step_mode && skip_step_beaks == 0) {
			/* Prompt */
			console_log_force("CONTROL", "Press ENTER to continue or type a "
				"number to skip more than one step");

			/* Read line */
			char input[64];
			uint8_t input_length = 0;

			/* While not enter is pressed */
			while((input[input_length++] = fgetc(stdin)) != '\n') {} 

			/* Terminate string */
			input[input_length-1] = 0;

			/* If a number was typed, parse number and skip next steps */
			if(input_length > 0) {
				skip_step_beaks = atoi(input);

			}
		}

		/* Decrement skip_step_beaks */
		if(skip_step_beaks > 0) {
			skip_step_beaks--;
		}
	}

	/* Print results */
	console_log("RESULT", 
			"================================================================" 
			"===============");
	console_log_force("RESULT", "%-20s %" PRIu64, 
		"Total CPU time:", cpu_time-1);
	console_log_force("RESULT", "%-20s %" PRIu64, 
		"Total page faults:", memory_page_faults);
	console_log_force("RESULT", "%-20s %" PRIu64, 
		"Total cpu idle time:", cpu_stall_counter);	
}

/* Loads the process file and puts the proceeses into 
   process_queue_head */
void control_create_process_arrival_queue(char *trace_file_name) {
	/* Open file, handle errors */
	FILE* trace_file;
	if((trace_file = fopen(trace_file_name, "r")) == NULL) {
		console_error("CONTROL", 
			"ERROR: Unable to read process trace file \"%s\" (%s)",
			trace_file_name, strerror(errno));
		exit(ERR_NO_UNABLE_OPEN_PROCESS_TRACE);
	}

	/* Iterate over all lines */
	char line[1024];
	int32_t line_result = 0;
	uint32_t pid_counter = 0;

	for(;;) {
		/* Read current line */
		line_result = read_line(trace_file, line);

		/* If the line is empty, we are at the end of the file, break */
		if(line_result == 0) {
			break;
		}

		/* If a negative value is returned, a error occured */
		if(line_result < 0) {
			console_error("CONTROL", 
				"ERROR: Unable to read memory trace file \"%s\" (%s)",
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
    			console_error("CONTROL", 
    				"ERROR: error while parsing trace file! (%s)",
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
    	pcb->pid = pid_counter++;
    	memcpy(pcb->name, values, strlen(values[0])+1);
    	pcb->cpu_time = 0;
    	pcb->start_time = SECS_TO_TICKS(atof(values[1]));
    	pcb->waiting_time = 0;
    	pcb->io_count = atof(values[3]);
    	pcb->page_faults = 0;
    	pcb->end_time = -1;
    	pcb->instruction_pointer = NULL;
    	pcb->next = NULL;

    	/* Print result */
    	console_log("CONTROL", 
    		"SUCESS: created process control block for \"%s\"", pcb->name);
    	console_log("CONTROL", "\tstart_time = %" PRIu64, pcb->start_time);
		console_log("CONTROL", "\tcpu_time = %" PRIu64, pcb->cpu_time);
		console_log("CONTROL", "\tio_count = %" PRIu64, pcb->io_count);

    	/* Link pcb to previous in queue or set as head if queue is empty */
    	if(process_queue_head == NULL) {
    		process_queue_head = pcb;
    	} else {
    		process_queue_tail->next = pcb;
    	}

    	/* Set pcb as new tail in queue */
    	process_queue_tail = pcb;
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
	sprintf(full_path, "%s%s.mem", dir, process->name);

	/* Open file */
	FILE* trace_file;
	if((trace_file = fopen(full_path, "r")) == NULL) {
		console_error("CONTROL", 
			"ERROR: Unable to read memory trace file \"%s\" (%s)",
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
			console_error("CONTROL", 
				"ERROR: Unable to read memory trace file \"%s\" (%s)",
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