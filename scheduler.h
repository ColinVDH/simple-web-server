
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "rcb.h"

void scheduler_init( char * name );
void scheduler_add( rcb_t * rcb );
rcb_t * scheduler_get();

#endif
