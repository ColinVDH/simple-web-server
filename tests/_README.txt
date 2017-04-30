This file gives a brief explanation of test0 - test9.

The tests developed for this Multithreaded Scheduling Web Server were aimed for 
ensuring that each scheduler type worked properly with one thread running. 

That is to say that the server was started with the command 
"./sws <port #> <scheduler> 1"

The tests are confirmed to work with one thread on Linux Mint and Bash for 
Windows. 
Some issues were encountered with the provided virtual machine though.
Up to 100 threads are supported. 

Note that they are currently set to use port 8080. Some tests are slow to 
provide output with sjf. 
