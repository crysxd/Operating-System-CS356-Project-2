/*
 * control.h
 * Author: Christian Würthner
 * Description: control for project 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <limits.h>
#include "scheduler.h"
#include "cpu.h"
#include "memory.h"
#include "pager.h"
#include "error_codes.h"
#include "console.h"

/* Queue for arriving procesess. Feed from trace file */ 
static pcb_t *process_queue_head = NULL;
static pcb_t *process_queue_tail = NULL;

uint64_t cpu_time = 0;
bool control_step_mode = false;

/* Loads the process file and puts the proceeses into 
   process_queue_head */
void control_create_process_arrival_queue(char *trace_file_name);

/* Loads the memory trace file for the given process from the given directory */
void control_load_memory_trace(pcb_t *process, char *dir);

/* Reads a single line from the file pointer */
int32_t read_line(FILE *file, char *dest);