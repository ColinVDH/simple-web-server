#ifndef RCB_QUEUE_H
#define RCB_QUEUE_H

#include "rcb.h"

typedef struct node {
	rcb_t *val;
	struct node *next;
} node_t;

typedef struct queue {
	node_t *head;
	node_t *tail;
} queue;

void enqueue(queue *q, rcb_t *rcb);
void sjf_enqueue(queue *q, rcb_t *rcb);
rcb_t * dequeue(queue *q);
int queue_empty(queue *q);

#endif
