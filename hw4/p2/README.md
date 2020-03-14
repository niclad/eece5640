# Binning using MPI
Written by Nicolas Tedori. Due March 17, 2020 at 0800.

These programs, written in C++ and using MPI, bins integer values in the range from 1 to 1000, inclusive.

## Files
1. [tedori_hw4p2a.cpp](tedori_hw4p2a.cpp): This bins (# bins user-defined) numbers (the amount of which is user-defined) on a user-defined number of nodes by chunking the number list for each node.
2. [tedori_hw4p2b.cpp](tedori_hw4p2b.cpp): This bins (# bins user-defined) numbers (the amount of which is user-defined) on a user-defined number of nodes by chunking the bin list for each node.
3. [Makefile](Makefile): Compiles both C++ codefiles. Outputs are of the same name as the input files without the filetype suffix.
4. [graphing.py](graphing.py): Graphs the results collected from running the programs.
5. [submit.sh](submit.sh): Submits a job using Slurms `sbatch`.

## Running code
The codefile, `tedori_hw4p2a.cpp` can be compiled using `bash$ make`. This will generate an executable with the same name as the code file, `tedori_hw4p2a`.

The codefile, `tedori_hw4p2b.cpp` can be compiled using `bash$ make binchunk`. This will generate an executable with the same name as the code file, `tedori_hw4p2b`.

To run either program, the executable requires a commandline input for the amount of numbers to bin, the number of nodes to run on, and the number of bins. So, when running this program, the command is `bash$ ./tedori_hw4p2<a|b> <amount_of_numbers> <number_of_nodes> <number_of_bins>`, where each input is determined by the used. Note that this must be done using Slurm (i.e. `srun` or `sbatch`) where the number of nodes defined in slurm *must* be equal to program input argument `<number_of_nodes>`. See [output](https://github.com/niclad/eece5640/tree/master/hw4/p2#output) for an example.

This program is run on the Discovery cluster using Slurm. As such, the number of processes is set using `sbatch` or `srun`.

For my own tests, this program was run using `sbatch`. The script used to submit the job is under [submit.sh](submit.sh). For this file, the `--node` setting should always be equal to the number of nodes passed as an input argument. See below for more info (the variable *X* should always be equivalent). 

submit.sh:
```
#!/bin/bash
#
#SBATCH --export=ALL
#SBATCH --partition=short	
#SBATCH --nodes=X
#SBATCH --tasks-per-node=1
#SBATCH --output=test.out
#SBATCH --time=7:30

mpirun --mca btl_base_warn_component_unused 0 ./binning 1000 X 10
```

See [Slurm](https://slurm.schedmd.com/overview.html) for more information on executing with `srun` or `sbatch`.

## Output
An example output for the command `bash$ sbatch execute.sh`, where `execute.sh` is:

```
#!/bin/bash
#
#SBATCH --export=ALL
#SBATCH --partition=short
#SBATCH --nodes=4
#SBATCH --tasks-per-node=1
#SBATCH --output=test.out
#SBATCH --time=7:30

mpirun --mca btl_base_warn_component_unused 0 ./tedori_hw4p2a 100000 4 10
```

The running time is only for the time required for the nodes to bin. Not to generate the numbers or other facets of the program.

The output of `sbatch execute.sh`:
```
Num processes: 4
N: 100000
# nodes: 4
fill working
Bins: 10
fill working
fill working
fill working
working
working
Numbers per node: 25000
0: 0.0009828 sec
2: 0.000991343 sec
3: 0.000956029 sec
1: 0.00114373 sec
Bin counts for 10 bins for numbers in range [1, 1000]
Bin    1:******************* 9887
Bin    2:******************* 9846
Bin    3:******************** 10050
Bin    4:******************* 9833
Bin    5:******************** 10054
Bin    6:******************* 9938
Bin    7:******************* 9947
Bin    8:******************** 10202
Bin    9:******************** 10148
Bin   10:******************** 10095
Total numbers binned: 100000
Running time: 0.00114373 sec
FREEING MEMORY
      localNum     FREED
     binCounts     FREED
totalBinCounts     FREED
       numbers     FREED
   tempNumbers     FREED
```