

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "rcb.h"
#include "scheduler.h"
#include "rcb_queue.h"
#include <assert.h>
#include <pthread.h>


pthread_mutex_t scheduler_mutex=PTHREAD_MUTEX_INITIALIZER; //mutex lock to ensure thread-safety
static char schedulers[3][5] = {"sjf", "rr", "mlfb"};
static char scheduler[] = "";                  
static int numschedulers = sizeof(schedulers)/sizeof(schedulers[0]);
static queue mlfb_queues[3];
static queue rr_queue;
static queue sjf_queue;
#define MLFB_HIGH 8192
#define MLFB_LOW 65536
#define RR 8192


void rr_add(rcb_t * rcb);
rcb_t* rr_get();
void mlfb_add(rcb_t * rcb);
rcb_t* mlfb_get();
void sjf_add(rcb_t * rcb);
rcb_t * sjf_get();

extern void scheduler_init( char * name ) {
  int i;
  for(i = 0; i < numschedulers; i++) {      
    if(strcmp(name, schedulers[i]) == 0) {  
      strcpy(scheduler, schedulers[i]);                  
      return;
    }
  }
  printf( "%s is not a scheduler\n", name );     
  abort();
}


extern void scheduler_add( rcb_t * rcb ) {
  assert(rcb);
  pthread_mutex_lock(&scheduler_mutex);
  assert(scheduler);
  if(strcmp(scheduler, "sjf") == 0){
    sjf_add(rcb);
  } else if(strcmp(scheduler, "rr") == 0){
    rr_add(rcb);
  }else if(strcmp(scheduler, "mlfb") == 0){
    mlfb_add(rcb);
  }
  pthread_mutex_unlock(&scheduler_mutex);
}


extern rcb_t * scheduler_get() {
  pthread_mutex_lock(&scheduler_mutex);
  rcb_t * rcb;
  assert(scheduler);
  if(strcmp(scheduler, "sjf") == 0){
    rcb=sjf_get();
  } else if(strcmp(scheduler, "rr") == 0){
    rcb=rr_get();
  }else if(strcmp(scheduler, "mlfb") == 0){
    rcb=mlfb_get();
  } else {
    rcb=NULL;
  }
  pthread_mutex_unlock(&scheduler_mutex);
  return rcb;
}




void rr_add(rcb_t * rcb){
  rcb -> quantum = RR;
  enqueue(&rr_queue, rcb);
}

rcb_t* rr_get(){
  return dequeue(&rr_queue);
}



void mlfb_add( rcb_t * rcb ) {
  assert( rcb );
  /* if not yet in 1 of the 3 mlfb_queues, place in 8KB level */
  if ( rcb->quantum == 0 ) {
    rcb->quantum = MLFB_HIGH;
    enqueue( &mlfb_queues[0], rcb );
    // Testing
    printf( "Request %d in 8KB Level.\n", rcb->sequence);

  } else if ( rcb->quantum == MLFB_HIGH ) {
    /* if already in 8KB level, move to 64KB level */
    rcb->quantum = MLFB_LOW;

    enqueue( &mlfb_queues[1], rcb );

// Testing
    printf( "Request %d in 64KB Level.\n", rcb->sequence);

  } else {
    /* place in RR level */
    rcb->quantum = MLFB_LOW;
    enqueue( &mlfb_queues[2], rcb );

// Testing
    printf( "Request %d in 8KB Level.\n", rcb->sequence);

  }
}
rcb_t * mlfb_get(){
  int x;
  rcb_t *rcb;

  for ( x = 0; x < 3; x++ ){
    rcb = dequeue( &mlfb_queues[x] );
    if (rcb) return rcb;
  }
  return NULL;
}


void sjf_add(rcb_t * rcb){
  sjf_enqueue(&sjf_queue, rcb);
}

rcb_t* sjf_get(){
  return dequeue(&sjf_queue);
}