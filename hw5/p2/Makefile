TARGET = mpiAuthors
GRAPH = cudaGraphing
CC = nvcc
CVER = -std=c++11

default: $(TARGET).cu
	$(CC) $(CVER) $(TARGET).cu -o cudaAuthors

omp: ompAuthors.cu
	$(CC) -Xcompiler -fopenmp $(CVER) ompAuthors.cu -o ompAuthors

graph: $(GRAPH).cu
	$(CC) $(CVER) $(GRAPH).cu -o cudaGraph

test: example.cpp
	$(CC) $(CVER) example.cpp -o test
