This file explains how to compile and run Group 13's Multithreaded 
Scheduling Web Server.

To compile -> run command "make" from <sws> directory within Terminal.

To run server -> within the <sws> directory in Terminal, the server can be 
started with the following command "./sws <port#> <scheduler> <#ofthreads>"

	- valid port number are in the range of 1024 to 65335

	- server limits the number of threads to less than or equal to 100

To run client connection -> enter command "./hydra.py < inputfile > outputfile"
Command may require prefix "python2"

For tests to execute, requested files (i.e., 5kb.txt) must be located in the 
directory containing simple web server. For easy use, place contents of "test data files"
in source code, then run tests.
Input and output files can be referred to with a path relative to the current 
directory.
See README in tests for more.

Caching is included as simplecache, which is a partial implementation. It is not connected
and doesn't work right now.