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
#include "scheduler.h"
#include "cpu.h"
#include "memory.h"
#include "pager.h"
#include "error_codes.h"

/* Queue for arriving procesess. Feed from trace file */ 
static pcb_t *control_process_arrival_queue_head = NULL;
static pcb_t *control_process_arrival_queue_tail = NULL;

void control_create_process_arrival_queue(char *trace_file_name);