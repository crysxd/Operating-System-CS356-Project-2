/*
 * memory.h
 * Author: Christian Würthner
 * Description: memory for project 2
 */
 
#ifndef __MEMORY__
#define __MEMORY__

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include "types.h"

#define MEMORY_FRAME_COUNT 5

/* Include needed variables */
extern uint64_t cpu_time;
extern pcb_t *scheduler_running;

/* Trap if a page fault happens */
extern trap memory_page_fault;

/* The page table */
extern frame_t *memory_map;

/* This method should be called once at atrup as it is emptying the entire 
   memory */
void memory_init();

/* Accesess the memory and raises a page fault if the requested page is not 
   in memory */
bool memory_access(uint32_t address);

#endif