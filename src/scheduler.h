/*
 * File: sws.c
 * Author: 			ID:
 *	Ryan Murchison		B00605559
 *	Colin Reagh		B00592295
 * 	Colin Vandenhof		B00320357
 * Purpose: This file contains the declarations for the scheduler. The 
 * 		scheduler_init() function initializes the string name 
 * 		for the scheduler. The scheduler_add() function add an 
 * 		rcb to the appropriate scheduler. The scheduler_get() 
 * 		function returns the next rcb from the appropriate scheduler.
 */


#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "rcb.h"
#include "rcb_queue.h"

void scheduler_init( char * name );

void scheduler_add( rcb_t * rcb );

rcb_t * scheduler_get();

#endif
