# Homework 3, Problem 3
OpenMP and pthreads Dining Philosophers implementation and comparison. This program uses OpenMP to time and run the Dining Philosophers problem. Same for pthreads. The code included is different than in homework 2 - the code is altered so that operations are similar and to allowing for accurate timing.

## Files
[Makefile](./Makefile) Used to compile the C++ code files [tedori_hw3p3_omp.cpp](./tedori_hw3p_omp.cpp) and [tedori_hw3p3_pthread.cpp](./tedori_hw3p3_pthread.cpp).

[tedori_hw3p3_omp.cpp](./tedori_hw3p_omp.cpp) Dining Philosophers using OpenMP.

[tedori_hw3p3_pthread.cpp](./tedori_hw3p3_pthread.cpp) Dining Philosophers using pthreads.

## Compiling
Use the Makefile to compile the C++ files described above. `$ make` will compile tedori_hw3p3_omp.cpp and make a file named `omp_phil`. `$ make pthread_phil` will compile tedori_hw3p4_reduc.cpp and make a file named `pthread_phil`.

See the Makefile for more information about compilation settings. Uses OpenMP 4.5.

## Running 
The files require no input and will run in bash using `$ ./<file_name>`. Both files will print the maximum threads available and which threads are running.