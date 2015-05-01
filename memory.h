/*
 * memory.h
 * Author: Christian Würthner
 * Description: cpu for project 2
 */
 
#ifndef __MEMORY__
#define __MEMORY__

#include <stdio.h>
#include <inttypes.h>
#include "types.h"

void memory_tick();

void memory_access(uint32_t address, trap page_fault);

#endif