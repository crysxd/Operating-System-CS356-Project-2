/*
 * memory.h
 * Author: Christian Würthner
 * Description: cpu for project 2
 */
 
#ifndef __MEMORY__
#define __MEMORY__

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include "types.h"

#define MEMORY_FRAME_COUNT 50

/* Trap if a page fault happens */
extern trap memory_page_fault;

/* Structure to represent a frame in memory */
typedef struct {
	uint32_t page_number;
	uint64_t time_loaded;
	uint64_t time_used;
	bool unused;
} frame_t;

/* The page table */
static frame_t memory_map[MEMORY_FRAME_COUNT];


/* Accesess the memory and raises a page fault if the requested page is not 
   in memory */
void memory_access(uint32_t address, trap page_fault);

#endif