# Dart Board Method Pi Estimation
Written by Nicolas Tedori. Due March 17, 2020 at 0800.

This program, written in C++, estimates the value of [Pi](https://en.wikipedia.org/wiki/Pi) using the dartboard method. This program prints the results and error (calculated using the C++ implementation of Pi) and the execution time of the program. This program esitmates pi concurrently using MPI.

## Files
1. [tedori_hw4p1.cpp](tedori_hw4p1.cpp): Pi estimation program
2. [mpiPi.cpp](mpiPi.cpp): A Pi estimation using the same method, provided by Oak Ridge National Laboratory. More info from ORNL available [here](https://www.olcf.ornl.gov/tutorials/monte-carlo-pi/). This code has been altered to compare timing with [tedori_hw4p1.cpp](tedori_hw4p1.cpp).
3. [Makefile](Makefile): Compiles [tedori_hw4p1.cpp](tedori_hw4p1.cpp). [mpiPi.cpp](mpiPi.cpp) is not compiled through this Makefile.

## Running code
The codefile, `tedori_hw4p1.cpp` can be compiled using `bash$ make`. This will generate an executable with the same name as the code file, `tedori_hw4p1`.

To run the program, the executable requires a commandline input for the number of darts. So, when running this program, the command is `bash$ ./tedori_hw4p1 <number_of_darts>`, where `<number_of_darts>` is determined by the user. E.g. a value of `1000000000` for `<number_of_darts>` will produce a value of Pi ~3.1416 with an error of ~0.0019% (compared to `M_PI` in `cmath`).

This program is run on the Discovery cluster using Slurm. As such, the number of processes is set using `sbatch` or `srun`.

To run using `srun`, the following command can be used: `bash$ srun --pty --export=ALL --nodes=1 --tasks-per-node=<number_of_tasks> mpirun --mca btl_base_warn_component_unused 0 ./tedori_hw4p1 <number_of_darts>`. For the purposes of my own experiments, I tested the execution for {`<number_of_task>` | 4, 8, 16, 24}.

See [Slurm](https://slurm.schedmd.com/overview.html) for more information on executing with `srun` or `sbatch`.

## Output
An example output for the command `bash$ srun --pty --export=ALL --nodes=1 --tasks-per-node=16 mpirun --mca btl_base_warn_component_unused 0 ./piDarts 100000` is below:

```
Num processes: 16
Darts per process: 6250
15 time: 0.000298243
1 time: 0.00029649
3 time: 0.000295321
5 time: 0.000298988
6 time: 0.000295503
7 time: 0.000295796
9 time: 0.000297032
11 time: 0.000301102
12 time: 0.000299728
13 time: 0.000299415
14 time: 0.000296377
10 time: 0.000300383
0 time: 0.000300009
2 time: 0.000299703
4 time: 0.000299774
8 time: 0.000314103
totalCircle: 79280
totalDarts: 100000
Running time: 0.000314103 seconds
Pi estimation: 3.1711999999999998...
Error (using M_PI): 0.94243%
```