#include "rcb_queue.h"


static queue sjf_queue;

void sjf_add(rcb_t * rcb){
	sjf_enqueue(&sjf_queue, rcb);
}

rcb_t* sjf_get(){
	return dequeue(&sjf_queue);
}