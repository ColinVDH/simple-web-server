# simple-web-server
Simple web server written in C for CSCI 3120 Operating Systems, Winter 2017. Original code by Alex Brodsky. Job scheduling and multi-threading implemented by Ryan Murchison, Colin Reagh, and Colin Vandenhof.


### Features
* responds to GET requests from clients.
* three scheduling algorithms are implemented: 
  1. **Shortest job first**: process jobs in queue by the size of the file requested.
  2. **Round robin**: send a fixed quantum of data from the file, and if the job is not finished, add it back to the end of the queue.
  3. **Multi-level feedback queue**: process jobs in multiple queues, each assigned a priority and a quantum. Jobs are always taken from the highest priority queue. If, after the quantum is processed, the job is not finished, it placed into the next lower priority queue. The lowest priority queue is round robin.
* multi-threading: multiple threads can work concurrently to process jobs.


### Usage
After compiling the source code using the provided Makefile, initiate the server through the command line with: 
```
./sws <port#> <SJF/RR/MLFQ> <#threads>
```
where <port#> is the port number of the server (between 1024 and 65335, since server is being run as a user-level process), <SJF/RR/MLFQ> is the scheduler, and <#threads> is the number of workers. The host is by default localhost. Clients can send requests to the server in the following form: 
```
GET <file> HTTP/1.1
```
where <file> is a / followed by the path of the file. Our simple web server ignores the rest of the
request, including the request headers, the empty line, and the optional message body. 
