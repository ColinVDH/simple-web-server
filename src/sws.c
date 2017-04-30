/*
 * File: sws.c
 * Author: 			ID:
 *	Ryan Murchison		B00605559
 *	Colin Reagh		B00592295
 * 	Colin Vandenhof		B00320357
 * Purpose: This file contains the implementation of a multithreaded simple web server.
 * 		It consists of several functions: main() which contains the 
 * 		main loop accept client connections, rcb_init(), in which 
 * 		rcb's are initialized, rcb_process(), in which the rcb is 
 * 		processed, and thread_loop(), in which threads process the 
 * 		requests.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/stat.h>
#include <ctype.h>

#include "network.h"
#include "rcb.h"
#include "rcb_queue.h"
#include "scheduler.h"

#define PORT_MIN 1024
#define PORT_MAX 65335
#define MIN_REQUESTS 1
#define MAX_REQUESTS 100

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER; //lock for the print function
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER; //lock for the global counter
pthread_mutex_t work_queue_mutex = PTHREAD_MUTEX_INITIALIZER;

static int counter = 1; //initialize global counter to 1
static int num_requests = 0; //initialize number of active requests to 0.
static queue work_queue;
static pthread_cond_t new_request; //pthread conditional representing new requests.


/*
 * This function issues an HTTP response to the client, 
 * and then initializes an rcb, if possible. It returns 0 if the rcb was 
 * properly initialized and -1 otherwise. 
 */
int rcb_init( rcb_t *rcb )
{
  int length = 0;
  struct stat stats;
  char *filepath = NULL;
  char *request_type = NULL;
  FILE *file = NULL;

  memset( rcb->buffer, 0, MAX_HTTP_SIZE );		//allocate buffer
  read( rcb->client, rcb->buffer, MAX_HTTP_SIZE );	//read in the HTTP request into buffer
  
  request_type = strtok( rcb->buffer, " " );

  if ( request_type )
  {
	filepath = strtok( NULL, " " );
  }

  //no request type or filepath provided, inform client and return
  if(!filepath)
  {   
	length = sprintf(rcb->buffer, "HTTP/1.1 400 Bad request\n\n");
	write(rcb->client, rcb->buffer, length);
	return -1;
  } 

  if( strcmp( "GET", request_type ) == 0 )
  {
	// skip leading '/', copy filepath and open
	filepath++;
	strncpy(rcb->filepath, filepath, FILENAME_MAX);

	file = fopen(rcb->filepath, "r");

	//unable to open file, inform client and return
	if( !file )
	{
		length = sprintf( rcb->buffer, "HTTP/1.1 404 File not found\n\n" );
		write(rcb->client, rcb->buffer, length);
		return -1;
	}

	//able to get stats on the file, inform client and continue
	if( stat( rcb->filepath, &stats ) == 0 )
	{
		length = sprintf( rcb->buffer, "HTTP/1.1 200 OK\n\n" );
		write(rcb->client, rcb->buffer, length);

		rcb->file = file;		//set file pointer
		rcb->remaining = stats.st_size;	//set bytes remaining.

		pthread_mutex_lock( &counter_mutex );

		rcb->sequence = counter;	//set rcb number
		counter++;			//increment global counter

		pthread_mutex_unlock( &counter_mutex );

		return 0;			//rcb has been initialized
	} else {
		fclose(file);			//close file
	}
  }
  return -1;					//rcb not initialized
}


/*
 * This function processes the rcb by writing the appropriate number of bytes 
 * to the client file. 
 */
int rcb_process(rcb_t * rcb)
{
  int chunk, length, sent;

  if( rcb->remaining == 0 )
  {
	return 1;			//done returns true.
  } else if( rcb->quantum != 0 && rcb->remaining > rcb->quantum ) {
	chunk = rcb->quantum;		//chunk is the size of the quantum
  } else {
	chunk = rcb->remaining;		//otherwise chunk is the amount remaining.
  }

  sent = chunk;

  do {
	//if chunk is larger than the max http size, only read the max amount
	if (chunk>MAX_HTTP_SIZE)
	{
		length = fread( rcb->buffer, 1, MAX_HTTP_SIZE, rcb->file );
	} else {
	//otherwise read the "chunk" amount
		length = fread( rcb->buffer, 1, chunk, rcb->file );
	}

	// if fread failed sent error
	if( length < 1 )
	{
		// DEBUGGING: fprintf(stderr, "Error reading from buffer\n");
		return -1;
	} else if( length >= 1 ) {
		//write from buffer to client file.
		length = write( rcb->client, rcb->buffer, length );

		// if write failed sent error
		if( length < 1 )
		{
			// DEBUGGING: fprintf( stderr, "Error writing to buffer\n" );
			return -1;
		}

		//subtract the amount written from the chunk size
		chunk = chunk - length;
		//subtract the amount written from the RCB remaining bytes
		rcb->remaining = rcb->remaining - length;
	}
  }


  while( chunk > 0 );		//loop until the whole chunk has been sent ( chunk==0 )

  pthread_mutex_lock( &print_mutex ); 

  printf( "Sent <%d> bytes of file <%s>.\n", sent, rcb->filepath );
  fflush( stdout );

  pthread_mutex_unlock( &print_mutex );        

  return rcb->remaining <= 0;		//returns true if done. 
}


/* This function is the thread loop. If dequeues a request from the work queue, adds them 
 * to the scheduler, and then removes an item from the scheduler and processes it (putting it
 * back into the scheduler if necessary)
 */
void * thread_loop(void *myid)
{
  rcb_t * rcb;

  while( 1 )
  {
	rcb = NULL;

	pthread_mutex_lock( &work_queue_mutex );

	while( num_requests == 0 )		//if there are no outstanding requests

	//block until main thread enqueues a request
	pthread_cond_wait( &new_request, &work_queue_mutex );

	rcb = dequeue( &work_queue );		//dequeue next request

	pthread_mutex_unlock(&work_queue_mutex);

	//if there is a request
	if ( rcb )
	{
		//if the rcb was properly initialized
		if ( !rcb_init( rcb ) )
		{
			scheduler_add( rcb );	//add the rcb to the scheduler

			pthread_mutex_lock( &print_mutex );

			printf( "Request for file <%s> admitted.\n", rcb->filepath );
			fflush( stdout );

			pthread_mutex_unlock( &print_mutex );
		} else {
			// DEBUGGING: printf( "Freeing: RCB couldn't be initialized ");

			num_requests--;		//decrement number of active requests

			close( rcb->client );	//close client file
			free( rcb );		//free rcb memory
			rcb=NULL;
		}
	}

	rcb = scheduler_get();			//get next in schedule

	if( rcb )
	{
		int done = rcb_process( rcb ); 

	        if ( !done )
		{
		          scheduler_add( rcb );	//add back into scheduler
		} else {
			pthread_mutex_lock( &print_mutex );

			printf( "Request for file <%s> completed.\n", rcb->filepath );
			fflush( stdout );

			pthread_mutex_unlock( &print_mutex );

			fclose( rcb->file );	//close file associated with stream
			close( rcb->client );	//close file associated with descriptor
			num_requests--;		//reduce number of active requests
			free( rcb );		//free memory
			rcb = NULL;
		}
	}
  }
}


/* This is the main function. It takes in arguments from the user, then initializes the network port, 
 * the scheduler, and creates the appropriate number of threads. Then, it loops, continually checking for 
 * new requests and adding them to the work queue. 
 */
int main( int argc, char **argv ) {
  int port = -1;  
  int num_threads = -1;                                  
  int fd;
  int temp;

  if( ( argc < 4 ) || ( ( sscanf( argv[1], "%d", &port ) ) < 1 ) || ( sscanf( argv[3], "%d", &num_threads ) < 1 ) )
  {
	printf( "usage: sms <port> <scheduler> <num_threads>\n" );
	return 0;
  }

  temp = port;

  if( ( temp < 1024 ) || ( temp > PORT_MAX ) )
  {
	printf( "usage: valid port numbers <1024--65335>\n" );
	return 0;
  }

  temp = num_threads;

  if( ( temp < MIN_REQUESTS ) || ( temp > MAX_REQUESTS ) )
  {
	printf( "usage: valid number of threads <1--100>\n" );
	return 0;
  }

  for( temp = 0 ; argv[2][temp] ; temp++ )
  {
	argv[2][temp] = tolower( argv[2][temp] );
  }

  network_init( port );				//initialize network module
  scheduler_init( argv[2] );			//initialize scheduler.
  pthread_cond_init( &new_request, NULL );	//initialize condition variable

  pthread_t tid[num_threads];

  int id[num_threads];

  for( temp = 0; temp < num_threads; temp++ )
  {
	id[temp] = temp;

	if ( pthread_create( &tid[temp], NULL, thread_loop, ( void* )( intptr_t )( id + temp ) ) )
	{
		// DEBUGGING: fprintf( stderr, "Error creating thread\n" );
		return -2;
	}
  }

  while( 1 )
  {
	network_wait();				//wait for new requests

	//get clients
	for( fd = network_open(); fd >= 0; fd = network_open() )
	{
		while ( num_requests >= MAX_REQUESTS ){}	//wait until the number of requests goes below the maximum

		rcb_t * rcb = malloc( sizeof *rcb );		//allocate memory for an rcb
		rcb -> client = fd;				//client file descriptor
		rcb -> quantum = 0;				//no quantum set yet

		pthread_mutex_lock( &work_queue_mutex );

		enqueue( &work_queue, rcb );			//enqueue the rcb into work queue.
		num_requests++;

		pthread_cond_broadcast( &new_request );		//wake up any sleeping threads
		pthread_mutex_unlock( &work_queue_mutex );
	}
  }

  for( temp = 0; temp < num_threads; temp++ )
  {
	//join threads
	if( pthread_join( tid[temp], NULL ) )
	{
		// DEBUGGING: fprintf(stderr, "Error joining thread\n");
		return -3;
	}
  }

  return 0;
}


