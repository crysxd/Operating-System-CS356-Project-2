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
#include "console.h"

/* Include needed variables */
extern uint64_t cpu_time;
extern pcb_t *scheduler_running;

/* Counter for page faults */
extern uint64_t memory_page_faults;

/* Trap if a page fault happens */
extern trap memory_page_fault;

/* Number of available pages/frames */
extern uint32_t memory_frame_count;

/* The page table */
extern frame_t *inverted_page_table;

/* This method should be called once at atrup as it is emptying the entire 
   memory */
void memory_init();

/* Accesess the memory and raises a page fault if the requested page is not 
   in memory */
bool memory_access(uint32_t address);

/* Prints a overview over the current inverted page table */
void print_inverted_page_table();

#endif