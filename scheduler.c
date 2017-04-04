

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "rcb.h"
#include "scheduler.h"
#include "sjf.h"
#include "rr.h"
#include "mlfb.h"
#include <assert.h>
#include <pthread.h>


pthread_mutex_t scheduler_mutex=PTHREAD_MUTEX_INITIALIZER; //mutex lock to ensure thread-safety
static char schedulers[3][5] = {"sjf", "rr", "mlfb"};
static char scheduler[] = "";                  
static int numschedulers = sizeof(schedulers)/sizeof(schedulers[0]);


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
