/* 
 * File: sws.c
 * Author: Alex Brodsky
 * Purpose: This file contains the implementation of a simple web server.
 *          It consists of two functions: main() which contains the main 
 *          loop accept client connections, and serve_client(), which
 *          processes each client request.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/stat.h>
#include "network.h"
#include "rcb.h"
#include "rcb_queue.h"
#include "scheduler.h"
#define MAX_HTTP_SIZE 8192                 
#define MAX_REQUESTS 100

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t work_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static int counter=1;
static queue work_queue;
static int num_requests=0;
static pthread_cond_t new_request;



int rcb_init(rcb_t *rcb){
  int length = 0;                                      
  struct stat st;                                   
  char *req = NULL;                                 
  char *brk;                                        
  char *tmp;                                        
  FILE *fin = NULL;                                        

  memset(rcb->buffer, 0, MAX_HTTP_SIZE);
  if( read( rcb->client, rcb->buffer, MAX_HTTP_SIZE) <= 0 ) {    
    perror( "Error while reading request" );
    return 0;
  } 
  tmp = strtok_r( rcb->buffer, " ", &brk );             
  if( tmp && !strcmp( "GET", tmp ) ) {
    req = strtok_r( NULL, " ", &brk );
  }
 
  if( !req ) {                                      
    length = sprintf( rcb->buffer, "HTTP/1.1 400 Bad request\n\n" );
    write( rcb->client, rcb->buffer, length );                       
  } else {                                          
    req++;           
    strncpy(rcb->filepath, req, FILENAME_MAX);                               
    fin = fopen( req, "r" );                        
    if(!fin) {                                   
      length = sprintf( rcb->buffer, "HTTP/1.1 404 File not found\n\n" );  
      write( rcb->client, rcb->buffer, length );                     
    } else if( !fstat( fileno( fin ), &st ) ) {      //!stat( req, &st ) 
      length = sprintf( rcb->buffer, "HTTP/1.1 200 OK\n\n" );
      write( rcb->client, rcb->buffer, length );
      rcb->file = fin;
      rcb->remaining = st.st_size;
      
      pthread_mutex_lock(&counter_mutex); 
      rcb->sequence = counter;
      counter++;
      pthread_mutex_unlock(&counter_mutex); 
     
      return 1; //rcb has been initialized
    } else {
      fclose(fin); //close file 
    }
  } 

  return 0; //rcb not initialized
}

int rcb_process(rcb_t * rcb){
  int chunk, length, sent;
  if (rcb->remaining==0) return 1;  //done returns true.
  else if (rcb->quantum!=0 && rcb->remaining > rcb->quantum)
    chunk=rcb->quantum;
  else chunk=rcb->remaining;
  sent=chunk;
  do {         
    if (chunk>MAX_HTTP_SIZE)
      length=fread( rcb->buffer, 1, MAX_HTTP_SIZE, rcb->file );  
    else 
      length=fread( rcb->buffer, 1, chunk, rcb->file );  
    if( length < 1 ) {                             
        perror( "Error reading" );
        return 1;
    } else if( length > 0 ) {                      
      length = write( rcb->client, rcb->buffer, length );
      if( length < 1 ) {                           
        perror( "Error writing" );
        return 1; //done
      }
      chunk=chunk-length;
      rcb->remaining = rcb->remaining-length; 
    }
  } while( length == MAX_HTTP_SIZE && chunk > 0 );    
  pthread_mutex_lock(&print_mutex); 
  printf("Sent %d bytes of file %s.\n", sent, rcb->filepath);
  fflush(stdout);
  pthread_mutex_unlock(&print_mutex);        
  return rcb->remaining <= 0; //not done returns false
}


void * thread_init(void *myid){
 //   int id = (intptr_t) myid;
    rcb_t * rcb;
    while (1){
      rcb = NULL;
      pthread_mutex_lock(&work_queue_mutex);
      while (num_requests==0)
        pthread_cond_wait(&new_request, &work_queue_mutex);
      rcb = dequeue(&work_queue);
      pthread_mutex_unlock(&work_queue_mutex);
      if (rcb){
        if (rcb_init(rcb)) {  
          scheduler_add(rcb);   //add to scheduler
          pthread_mutex_lock(&print_mutex); 
          printf("Request for file %s admitted.\n", rcb->filepath);
          fflush(stdout);
          pthread_mutex_unlock(&print_mutex); 
        }
        else{
          printf("FREEING: RCB couldn't be initialized");
          num_requests--;
          close(rcb->client); //close client connection
          free(rcb); //free memory
          rcb=NULL;
        }
      }
      rcb = scheduler_get(); //get next in schedule
      if (rcb){
        int done=rcb_process(rcb); 
        if (!done){ 
          scheduler_add(rcb);  //add back into scheduler
        }
        else{
            printf("FREEING: done with RCB");
            pthread_mutex_lock(&print_mutex); 
            printf("Request for file %s (#%d) completed.\n", rcb->filepath, rcb->sequence);
            fflush(stdout);
            pthread_mutex_unlock(&print_mutex); 
            fclose( rcb->file ); //close file associated with stream
            close( rcb->client ); //close file associated with descriptor
            num_requests--; //reduce number of active requests
            free(rcb);  //free memory
            rcb=NULL;
        }
      }
    }
}




int main( int argc, char **argv ) {
  int port = -1;  
  int num_threads=-1;                                  
  char scheduler[4];
  int fd;
                              
  if (( argc != 4 ) || ( sscanf( argv[1], "%d", &port ) < 1 ) 
                   || (sscanf(argv[2], "%s", scheduler) < 1 )
                   || (sscanf(argv[3], "%d", &num_threads) < 1 )){
    printf( "usage: sms <port> <scheduler> <num_threads>\n" );
    return 0;
  }
 
  network_init(port);         //initialize network module 
  scheduler_init(scheduler);  //initialize scheduler.
  pthread_cond_init(&new_request, NULL); //initialize condition variable

  pthread_t tid[num_threads];
  int id[num_threads];
  int i;
  for (i = 0; i < num_threads; i++) {
    id[i] = i;
    if (pthread_create(&tid[i], NULL, thread_init, (void*)(intptr_t)(id+i))) {
      fprintf(stderr, "Error creating thread\n");
      return -2;
    }
  }

  while (1) { 
    network_wait();
    for(fd = network_open(); fd >= 0; fd = network_open()) { //get clients
      while (num_requests>=MAX_REQUESTS){}
      rcb_t * rcb = malloc(sizeof *rcb);
      rcb -> client = fd;
      rcb -> quantum = 0;
      pthread_mutex_lock(&work_queue_mutex);
      enqueue(&work_queue, rcb);
      num_requests++;
      pthread_cond_broadcast(&new_request);  //wake up any sleeping threads
      pthread_mutex_unlock(&work_queue_mutex);
    }
  }

  for (i = 0; i < num_threads; i++) {
    if (pthread_join(tid[i], NULL)) { //join thread
      fprintf(stderr, "Error joining thread\n");
      return -3;
    }
  }
  return 0;
}


