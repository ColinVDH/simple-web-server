/*
 * File: sws.c
 * Author: 			ID:
 *	Ryan Murchison		B00605559
 *	Colin Reagh		B00592295
 * 	Colin Vandenhof		B00320357
 * Purpose: This file contains the implementation of a scheduler. The 
 * 		scheduler_init() function initializes the string name 
 * 		for the scheduler. The scheduler_add() function add an 
 * 		rcb to the appropriate scheduler. The scheduler_get() 
 * 		function returns the next rcb from the appropriate scheduler.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#include "rcb.h"
#include "rcb_queue.h"
#include "scheduler.h"

#define MLFB_HIGH 8192		//MLFB - high priority number of bytes
#define MLFB_MED 65536		//MLFB - med priority number of bytes
#define MLFB_LOW 65536		//MLFB - low priority number of bytes
#define RR 8192			//RR - round robin quatum

pthread_mutex_t scheduler_mutex=PTHREAD_MUTEX_INITIALIZER;	//mutex lock to ensure thread-safety

static char schedulers[3][5] = {"sjf", "rr", "mlfb"}; 
static char scheduler[] = "";					//store initialized scheduler
static int numschedulers = sizeof( schedulers )/sizeof( schedulers[0] );

static queue mlfb_queues[3];					//MLFB queues (8KB, 64KB, RR)
static queue rr_queue;						//RR queue
static queue sjf_queue;						//SJF queue

void rr_add( rcb_t * rcb );
rcb_t* rr_get();

void mlfb_add( rcb_t * rcb );
rcb_t* mlfb_get();

void sjf_add( rcb_t * rcb );
rcb_t * sjf_get();

/*
 * This function initializes the scheduler (if possible) by setting the scheduler string.
 */
void scheduler_init( char * name )
{
  int temp;

  for(temp = 0; temp < numschedulers; temp++)
  {
	if( strcmp( name, schedulers[temp] ) == 0 )
	{
		strcpy( scheduler, schedulers[temp] );
		return;
	}
  }

  // DEBUGGING: fprintf( stderr, "%s is not a scheduler\n", name );     
  abort();
}

/*
 * This function takes in an rcb pointer and calls the appropriate 
 * scheduler function to add the rcb to the queue
 */
void scheduler_add( rcb_t * rcb )
{
  assert( rcb );

  pthread_mutex_lock( &scheduler_mutex );

  assert( scheduler );

  if(strcmp( scheduler, "sjf" ) == 0 )
  {
	sjf_add( rcb );
  } else if( strcmp( scheduler, "rr" ) == 0 ) {
	rr_add( rcb );
  } else if( strcmp( scheduler, "mlfb" ) == 0 ) {
    mlfb_add( rcb );
  }

  pthread_mutex_unlock(&scheduler_mutex); 
}

/* 
 * This function calls the appropriate scheduler function to return 
 * the next rcb from the queue.
 */
rcb_t * scheduler_get()
{
  pthread_mutex_lock( &scheduler_mutex );

  rcb_t * rcb;

  assert(scheduler);

  if( strcmp( scheduler, "sjf" ) == 0 )
  {
	rcb = sjf_get();
  } else if( strcmp( scheduler, "rr" ) == 0 ) {
	rcb = rr_get();
  } else if( strcmp( scheduler, "mlfb" ) == 0 ) {
	rcb = mlfb_get();
  } else {
    rcb = NULL;
  }

  pthread_mutex_unlock( &scheduler_mutex );

  return rcb;
}


/* 
 * This function adds an rcb to the round robin scheduler
 */
void rr_add(rcb_t * rcb)
{
  rcb -> quantum = RR; //set the quantum
  enqueue( &rr_queue, rcb );
}


/*
 * This function returns an rcb from the round robin scheduler
 */
rcb_t* rr_get()
{
  return dequeue( &rr_queue );
}


/*
 *  This function adds an rcb to the multi-level feedback scheduler
 */
void mlfb_add(rcb_t* rcb )
{
  assert( rcb );

  //if not yet in 1 of the 3 mlfb_queues, place in 8KB level 
  if ( rcb->quantum == 0 )
  {
	rcb->quantum = MLFB_HIGH;
	enqueue( &mlfb_queues[0], rcb );
  } else if ( rcb->quantum == MLFB_HIGH ) {
	rcb->quantum = MLFB_MED;		//if already in high priority, move to medium priority
	enqueue( &mlfb_queues[1], rcb );
  } else {
	rcb->quantum = MLFB_LOW;		//already in medium priority, move to low (RR) priority
	enqueue( &mlfb_queues[2], rcb );
  }
}

/*
 * This function returns an rcb from the multi-level feedback scheduler
 */
rcb_t* mlfb_get()
{
  int temp;
  rcb_t *rcb;

  for ( temp = 0; temp < 3; temp++ )
  {
	rcb = dequeue( &mlfb_queues[temp] );
	if( rcb ) return rcb;
  }

  return NULL;
}


/*
 * This function adds an rcb to the shortest-job-first scheduler
 */
void sjf_add( rcb_t * rcb )
{
  sjf_enqueue( &sjf_queue, rcb );
}


/*
 * This function returns an rcb from the shortest-job-first scheduler
 */
rcb_t* sjf_get()
{
  return dequeue( &sjf_queue );
}
