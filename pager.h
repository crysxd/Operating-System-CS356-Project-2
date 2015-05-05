/*
 * pager.h
 * Author: Christian Würthner
 * Description: pager for project 2
 */
 
#ifndef __PAGER__
#define __PAGER__

#include <stdio.h>
#include <inttypes.h>
#include "types.h"
#include "scheduler.h"
#include "memory.h"

#define PAGER_ALGORITHM_LRU

static uint64_t pager_consume_ticks = 0;

struct load_request {
	pcb_t *process;
	struct load_request *next;
};
typedef struct load_request load_request_t;

static load_request_t *load_request_queue_head = NULL;
static load_request_t *load_request_queue_tail = NULL;

void pager_tick();

/* Loads the page for the current instruction of the given process */
void pager_load(pcb_t *process);

/* Performs the next load operation from the load request queue */
void pager_perform_next_load();

#endif