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

void pager_tick();

void pager_load(uint32_t address, trap adress_error);

#endif