/*
 * types.h
 * Author: Christian Würthner
 * Description: header file containign all defined types
 */
 
#ifndef __TYPES__
#define __TYPES__

#define TICKS_PER_SEC 100.0
#define SECS_TO_TICKS(x) x*TICKS_PER_SEC
#define TICKS_TO_SECS(x) x/TICKS_PER_SEC

/* structure to make a linked list representing a memory trace file */
struct instruction {
	struct instruction *next;
	uint32_t address;
};
typedef struct instruction instruction_t;

/* pcb_t is a structure representing a simple process control block */
struct pcb {
	char *name;
	uint64_t cpu_time;
	uint64_t start_time;
	uint64_t waiting_time;
	uint64_t end_time;
	uint64_t io_count;
	instruction_t *instruction_pointer;
	struct pcb *next;
};
typedef struct pcb pcb_t;

/* void function pointer used to store function calls for traps */
typedef void (*trap)();

#endif