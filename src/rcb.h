/*
 * File: sws.c
 * Author: 			ID:
 *	Ryan Murchison		B00605559
 *	Colin Reagh		B00592295
 * 	Colin Vandenhof		B00320357
 * Purpose: RCB.h declares the structure of the Request Control Block 
 * 		of client's file requests.
 */

#ifndef RCB_H
#define RCB_H

#include <stdio.h>

#define MAX_HTTP_SIZE 8192 

//struct for an rcb. 
typedef struct rcb_ {
  char	buffer[MAX_HTTP_SIZE];	//rcb buffer string
  char	filepath[FILENAME_MAX];	//rcb filepath string
  int	sequence;		//the sequence # of the rcb
  int	client;			//client file descriptor
  FILE	* file;			//file pointer
  int	remaining;		//number of bytes remaining to be processed
  int	quantum;		//chunk of bytes allowable to process.
} rcb_t;

#endif
