#include "rcb_queue.h"
#include "rcb.h"

static queue rr_queue;

void rr_add(rcb_t * rcb){
	rcb -> quantum = 8192;
	enqueue(&rr_queue, rcb);
}

rcb_t* rr_get(){
	return dequeue(&rr_queue);
}


