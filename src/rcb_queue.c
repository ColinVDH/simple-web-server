/*
 * File: sws.c
 * Author: 			ID:
 *	Ryan Murchison		B00605559
 *	Colin Reagh		B00592295
 * 	Colin Vandenhof		B00320357
 * Purpose: rcb_queue.c contains 4 functions enqueue, sjf_enqueue, dequeue, queue_empty.
 */

#include <stdlib.h>

#include "rcb_queue.h"


/*
 * This function enqueues an rcb into the given queue.
 */
void enqueue( queue *q, rcb_t *rcb )
{
  node_t *temp = malloc( sizeof( node_t ) );	//allocate new node

  temp->val = rcb;				//set rcb
  temp->next = NULL;

  if( queue_empty( q ) )
  {
	q->head = temp; 			//queue empty, so set new RCB as the head
  } else {
	q->tail->next = temp;			//place it at the end (after current tail)
 }

  q->tail = temp;				//point tail to it
}


/*
 * This function enqueues an rcb into the given queue, 
 * according to shortest-job first. 
 */
void sjf_enqueue(queue *q, rcb_t *rcb)
{
  node_t *temp = malloc( sizeof( node_t ) );

  temp->val = rcb;					//set value of node
  temp->next = NULL;					//set next node

  if( queue_empty( q ) )
  {
	q->head = temp;					//update head
	q->tail = temp;					//update tail
  } else {
	//at least one entry
	node_t *prev = NULL;
	node_t *curr = q->head;

	while( curr != NULL && temp->val->remaining >= curr->val->remaining )
	{
	//while the current node is not NULL AND its value is greater than the new node
		prev = curr;				//increment previous node
		curr = curr->next;			//increment current node
	}

	temp->next = curr;				//found its place, set "next" of the new node to the current node.

	if( prev ) prev->next = temp;			//if prev exists, prev's "next" is temp
	if( q->head == temp->next ) q->head = temp;	//update head if necessary
	if( q->tail->next != NULL ) q->tail = temp;	//update tail if necessary
  }
}


/*
 * This function returns true if head pointer is NULL
 */
int queue_empty( queue *q )
{
  return !q->head;
}


/*
 * This function dequeues an rcb from the given queue. 
 * If the queue is empty, it returns NULL
 */
rcb_t * dequeue( queue *q )
{
  rcb_t *rcb = NULL;

  //queue is not empty
  if( !queue_empty( q ) )
  {
	rcb = q->head->val;		//get rcb at the head

	//if the head and the tail are the same (1 item)
	if (q->tail==q->head)
	{
		q->tail = NULL;		//tail is now NULL
	}

	node_t *temp = q->head->next;	//get the item after the current head

	free( q->head );		//free the node at the head

	q->head = temp;			//update head pointer
  }
  //else printf("deq: QUEUE EMPTY");
  return rcb;
}
