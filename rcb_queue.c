#include "rcb_queue.h"
#include "rcb.h"
#include <stdlib.h>


//enqueues rcb
void enqueue(queue *q, rcb_t *rcb) {
	node_t *temp = malloc(sizeof(node_t));
	temp->val = rcb;
	temp->next = NULL;
	if (queue_empty(q)){
		q->head = temp;
	}
	else{
		q->tail->next = temp;
	}
	q->tail = temp;
}

void sjf_enqueue(queue *q, rcb_t *rcb) {
	node_t *temp = malloc(sizeof(node_t));
	temp->val = rcb; //set value of node
	temp->next = NULL; //set next node
	if (queue_empty(q)){ 
		//printf("en: QUEUE EMPTY");
		q->head = temp; //update head
		q->tail = temp; //update tail
	}
	else{ //at least one entry
		node_t *prev = NULL; 
		node_t *curr = q->head;
		while (curr!=NULL && temp->val->remaining >= curr->val->remaining){ //while the current node is not NULL AND its value is greater than the new node
			prev = curr; //increment previous node
			curr = curr->next; //increment current node
		}
		temp->next=curr; //found its place, set "next" of the new node to the current node.
		if (prev) prev->next=temp; //if prev exists, prev's "next" is temp
		if (q->head==temp->next) q->head=temp; //update head if necessary
		if (q->tail->next!=NULL) q->tail=temp; //update tail if necessary
	}
}


int queue_empty(queue *q){
	return !q->head;
}

rcb_t * dequeue(queue *q) {
	rcb_t *rcb = NULL;
	if (!queue_empty(q)) {
		rcb = q->head->val;
		if (q->tail==q->head){
			q->tail=NULL;
		}
		node_t *temp = q->head->next;
		free(q->head);
		q->head = temp;
	}
	//else printf("deq: QUEUE EMPTY");
	return rcb;
}