# Targets & general dependencies
PROGRAM = sws
HEADERS = network.h rcb_queue.h rcb.h scheduler.h
OBJS = network.o rcb_queue.o scheduler.o sws.o
ADD_OBJS = 

# compilers, linkers, utilities, and flags
CC = gcc
CFLAGS = -Wall -g
COMPILE = $(CC) $(CFLAGS)
LINK = $(CC) $(CFLAGS) -o $@ 

# implicit rule to build .o from .c files
%.o: %.c $(HEADERS)
	$(COMPILE) -c -o $@ $<


# explicit rules
all: sws

$(PROGRAM): $(OBJS) $(ADD_OBJS)
	$(LINK) $(OBJS) $(ADD_OBJS) -lpthread -lrt

lib: sws_gold.o 
	 ar -r libxsws.a sws_gold.o

clean:
	rm -f *.o *.gch $(PROGRAM) 

zip:
	rm -f sws.zip
	zip sws.zip network.c network.h makefile
