# Dining Philosophers for EECE 5640
Written by Nicolas Tedori. Due February 7, 2020 at 0800.

This code will run the dining philosophers problem for a number of philosophers defined by the user.

All code is written in C++, parallelized with pthreads (_not_ C++11's `std::thread`). Makefile is provided. Compile with `bash$ make`.

### Code versions
[tedori_hw2p1.cpp](./tedori_hw2p1.cpp) will have philosophers think and eat for random durations.

[tedori_hw2p1_priority.cpp](./tedori_hw2p1_priority.cpp) gives priotity to one philosopher over the others.

[tedori_hw2p1_time.cpp](./tedori_hw2p1_time.cpp) forces the philosophers to eat for the same amount of time, but will not restrict their thinking time.

## Compiling code
A [Makefile](./Makefile) is provided with the code. However, there are several options which will generate different executables (see **Code versions**).

Using the command `bash$ make` will compile the default program, [tedori_hw2p1.cpp](./tedori_hw2p1.cpp).

Using the command `bash$ make priority` will compile the program that assigns a priority, [tedori_hw2p1_priority.cpp](./tedori_hw2p1_priority.cpp).

Using the command `bash$ make time` will compile the program that sets an eating time, [tedori_hw2p1_time.cpp](./tedori_hw2p1_time.cpp).

## Running code
Running the code follows a similar format as compiling. To run the default program, use `bash$ ./dining`. For the priority and time programs, run `bash$ ./priority` and `bash$ ./time`, respectively.