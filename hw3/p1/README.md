# Parallel Prime Calculator for EECE 5640
Written by Nicolas Tedori. Due February 7, 2020 at 0800.

This code will calculate prime numbers, up-to and including the upper-limit provided by the user.

All code is written in C++, parallelized with pthreads (_not_ C++11's `std::thread`). Makefile is provided. Compile with `bash$ make` and remove .exe's with `bash$ make clean`. 

## Running code

After running the exe, named *prime_calc* (i.e. `bash$ ./prime_calc`), a user will be prompted to input the upper limit. 1 is assumed prime and will be regarded as an invalid response. The maximum input value is the system's maximum *signed long int* value (though this number always caused a segmentation fault during my tests, likely due to memory sizes: an upper-limit of 2,147,483,647 would require an array of ~1GB).

The user is then prompted to input the number of threads to run the prime sorting on. The maximum number of threads is limited so that each thread has an appropriate number of values to operate on. Although not strictly necessary, this is more of a safety factor to ensure that no numbers are skipped and that the alignment between indices and values remains consistent. This was a problem that kept popping up. If the safety factor were removed and the number of threads exceeed that of the amount of values in the range [2, UPPER_LIMIT] (inclusive), then there's less than no benefit with the unnecessary thread spawning. With reasonably large numbers, though, a thread limit will likely be imposed by the system running the program.

The output shows the which thread is running on which segment of indices and values. This was used as debugging at first, but is handy to see what's happening. The amount of prime numbers found is displayed (as a benchmark, there are 50,847,534 between 1 and 1,000,000,000), as long as the first and last 10 values of all the prime numbers found.

For an upper-limit of 1 billion, 32 threads can find all ~51 million primes in under 10 seconds on the COE machines.

## Future improvements
1. Parallelize the prime calculation (not just the search).

2. Force segment sizes to depend on the page or cache size. Latter would improve locality.
   - This would unfortunately alter the way primes are calculate with could end up being a lateral change.