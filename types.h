/*
 * types.h
 * Author: Christian Würthner
 * Description: header file containign all defined types
 */
 
#ifndef __TYPES__
#define __TYPES__

#include <stdbool.h>
#include <inttypes.h>

#define TICKS_PER_SEC 100.0
#define SECS_TO_TICKS(x) x*TICKS_PER_SEC
#define TICKS_TO_SECS(x) x/TICKS_PER_SEC
#define PAGER_REPLACE_TIME 1000
#define SCHEDULER_CONTEXT_SWITCH_TIME 50

/* structure to make a linked list representing a memory trace file */
struct instruction {
	struct instruction *next;
	uint32_t address;
};
typedef struct instruction instruction_t;

/* pcb_t is a structure representing a simple process control block */
struct pcb {
	char *name;
	uint32_t pid;
	uint64_t cpu_time;
	uint64_t start_time;
	uint64_t waiting_time;
	uint64_t end_time;
	uint64_t io_count;
	uint64_t page_faults;
	instruction_t *instruction_pointer;
	struct pcb *next;
};
typedef struct pcb pcb_t;

/* Structure to represent a frame in memory */
typedef struct {
	uint32_t owner_pid;
	uint32_t page_number;
	uint64_t time_loaded;
	uint64_t time_used;
	bool used;
	bool used_since_load;
	bool empty;
} frame_t;

/* void function pointer used to store function calls for traps */
typedef void (*trap)();

#endif