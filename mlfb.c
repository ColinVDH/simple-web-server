/*
 * mlfb.c: 3 level; 8KB (high priority) , 64KB (lower priority) ,
 *      Round Robin (lowest priority)
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "rcb.h"
#include "rcb_queue.h"

#define HIGH 8192
#define LOW 65536

static queue levels[3];

void mlfb_add( rcb_t * rcb ) {
  assert( rcb );
  /* if not yet in 1 of the 3 levels, place in 8KB level */
  if ( rcb->quantum == 0 ) {
    rcb->quantum = HIGH;
    enqueue( &levels[0], rcb );
    // Testing
    printf( "Request %d in 8KB Level.\n", rcb->sequence);

  } else if ( rcb->quantum == HIGH ) {
    /* if already in 8KB level, move to 64KB level */
    rcb->quantum = LOW;

    enqueue( &levels[1], rcb );

// Testing
    printf( "Request %d in 64KB Level.\n", rcb->sequence);

  } else {
    /* place in RR level */
    rcb->quantum = LOW;
    enqueue( &levels[2], rcb );

// Testing
    printf( "Request %d in 8KB Level.\n", rcb->sequence);

  }
}


rcb_t * mlfb_get(){
  int x;
  rcb_t *rcb;

  for ( x = 0; x < 3; x++ ){
    rcb = dequeue( &levels[x] );
    if (rcb) return rcb;
  }
  return NULL;
}


