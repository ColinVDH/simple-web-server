
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "rcb.h"

extern void scheduler_init( char * name );

extern void scheduler_add( rcb_t * rcb );

extern rcb_t * scheduler_get();

#endif
