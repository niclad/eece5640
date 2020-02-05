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

## Output
Output shows the state of each philosopher as it changes. So a philosopher can be thinking, eating, or done eating (putting down the forks).

An example output is presented below for an input of 5 philosophers:
```
-bash-4.2$ ./dining
DINING PHILOSOPHERS
Number of philosophers: 5
Philosopher 2 of 5 is thinking.
Philosopher 4 of 5 is thinking.
Philosopher 5 of 5 is thinking.
Philosopher 1 of 5 is thinking.
Philosopher 3 of 5 is thinking.
Philosopher 2 of 5 is EATING with forks 2 and 3.
```
The output is folded for brevity, though the output looks similar.
```
Philosopher 2 of 5 is thinking.
Philosopher 3 of 5 is EATING with forks 3 and 4.
PHILOSOPHERS ARE FINISHING.
Philosopher 5 of 5 is done EATING. Freeing forks 5 and 1.
Philosopher 1 of 5 is EATING with forks 1 and 2.
Philosopher 3 of 5 is done EATING. Freeing forks 3 and 4.
Philosopher 4 of 5 is EATING with forks 4 and 5.
Philosopher 1 of 5 is done EATING. Freeing forks 1 and 2.
Philosopher 4 of 5 is done EATING. Freeing forks 4 and 5.
```