/*
 * File: sws.c
 * Author: 			ID:
 *	Ryan Murchison		B00605559
 *	Colin Reagh		B00592295
 * 	Colin Vandenhof		B00320357
 * Purpose: rcb_queue.h declares the structure of the queue and the nodes 
 * 		of the queue of the queue, as well as declaration for 
 *		enqueueing/dequeueing functions plus queue size verification.
 */

#ifndef RCB_QUEUE_H
#define RCB_QUEUE_H

#include "rcb.h"


//struct for a node in the queue
typedef struct node { 
	rcb_t *val;		//pointer to rcb
	struct node *next;	//pointer to next node
} node_t;

//struct for a queue
typedef struct queue {
	node_t *head;		//pointer to the head of the queue
	node_t *tail;		//pointer to the tail of the queue
} queue;

void enqueue( queue *q, rcb_t *rcb );

void sjf_enqueue( queue *q, rcb_t *rcb );

rcb_t * dequeue( queue *q );

int queue_empty( queue *q );

#endif
