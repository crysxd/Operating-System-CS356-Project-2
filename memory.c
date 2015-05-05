/*
 * memory.c
 * Author: Christian Würthner
 * Description: memory for project 2
 */

#include "memory.h"

/* Define in header declared fields */
trap memory_page_fault = NULL;
frame_t *memory_map = NULL;
uint64_t page_faults = 0;

/* This method should be called once at atrup as it is emptying the entire 
   memory */
void memory_init() {
	/* Create memory map */
	memory_map = malloc(sizeof(frame_t) * MEMORY_FRAME_COUNT);

	/* Iterate over memory_map and set default values */
	for(uint32_t i=0; i<MEMORY_FRAME_COUNT; i++) {
		memory_map[i].empty = true;
	}
}

/* Accesess the memory and raises a page fault if the requested page is not 
   in memory. Returns true if a page fault happend. */
bool memory_access(uint32_t address) {
	uint32_t frame = 0;
	bool page_found = false;

	/* Iterate over memory_map and search for frame */
	for(uint32_t i=0; i<MEMORY_FRAME_COUNT; i++) {
		if(!memory_map[i].empty && 
			memory_map[i].page_number == address &&
			memory_map[i].owner_pid == scheduler_running->pid) {
			frame = i;
			page_found = true;
		}
	}

	/* If we didn't find the page, it's a page fault */
	if(!page_found) {
		printf("[MEMORY] Page fault for page %d\n", address);
		page_faults++;
		memory_page_fault();
		return true;
	}

	/* If we found it, update last access time and set used to true */
	else {
		memory_map[frame].time_used = cpu_time;
		memory_map[frame].used = true;
		return false;
	}
}


