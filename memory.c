/*
 * memory.c
 * Author: Christian Würthner
 * Description: memory for project 2
 */

#include "memory.h"

/* Trap if a page fault happens */
trap cpu_page_fault = NULL

/* Accesess the memory and raises a page fault if the requested page is not 
   in memory */
void memory_access(uint32_t address, trap page_fault) {
	
}


