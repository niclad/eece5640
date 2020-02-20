# Homework 3, Problem 4
OpenMP vector-matrix multiplication. This program uses OpenMP to parallely multiply a vector by a matrix. There are two versions: one that runs the multiplication function on multiple threads and another that uses OpenMP reduction for the element calculation loops.

## Files
[Makefile](./Makefile) Used to compile the C++ code files [tedori_hw3p4.cpp](./tedori_hw3p4.cpp) and [tedori_hw3p4_reduc.cpp](./tedori_hw3p4_reduc.cpp).

[tedori_hw3p4.cpp](./tedori_hw3p4.cpp) Parallelize the vector-matrix multiplication function.

[tedori_hw3p4_reduc.cpp](./tedori_hw3p4_reduc.cpp) Uses reduction for element calculations in the output vector.

## Compiling
Use the Makefile to compile the C++ files described above. `$ make` will compile tedori_hw3p4.cpp and make a file named `matrix_mult`. `$ make reduction` will compile tedori_hw3p4_reduc.cpp and make a file named `matrix_reduc`.

See the Makefile for more information about compilation settings. Note, however, that all

## Running 
The files require no input and will run in bash using `$ ./<file_name>`. Both files will print the maximum threads available and which threads are running. `matrix_reduc` will output all the threads in the reduction; this the makes the output quite verbose.