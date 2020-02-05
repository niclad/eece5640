# Dining Philosophers Makefile

# compiler and flag information
CC=g++
CFLAGS = -pthread -Wall
FILENAME = tedori_hw2p1.cpp 
TIMENAME = tedori_hw2p1_time.cpp
PRIONAME = tedori_hw2p1_priority.cpp
CVER = -std=c++11

default: dining

dining: $(FILENAME)
	$(CC) $(CFLAGS) -o dining $(CVER) $(FILENAME)

priority: $(PRIONAME)
	$(CC) $(CFLAGS) -o priority $(CVER) $(PRIONAME)

time: $(TIMENAME)
	$(CC) $(CFLAGS) -o time $(CVER) $(TIMENAME)

clean:
	rm *.exe
