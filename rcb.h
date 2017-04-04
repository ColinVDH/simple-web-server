#ifndef RCB_H
#define RCB_H

#include <stdio.h>
#define MAX_HTTP_SIZE 8192 

typedef struct rcb_ {
  char   	buffer[MAX_HTTP_SIZE];
  char 		filepath[FILENAME_MAX];
  int       sequence;   
  int       client;
  FILE *    file;    
  int     	remaining;   
  int       quantum;      
} rcb_t;

#endif