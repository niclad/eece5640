# Makefike prime count program

# compiler and flag information
CC=g++
CFLAGS = -pthread -Wall
FILENAME = tedori_hw2p2.cpp 
CVER = -std=c++11

default: prime_calc

prime_calc: $(FILENAME)
	$(CC) $(CFLAGS) -o prime_calc $(CVER) $(FILENAME)

clean:
	$(RM) prime_calc
