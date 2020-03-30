# MPI Author Statistics
Written by Nicolas Tedori. Due April 03, 2020 at 0800.

These programs, written in C++, use MPI to read a [file](dblp-co-authors.txt) containing author information and determine the authors with the most co-authors and graph the distribution of authors with a number of co-authors.

## Files
The files in this directory

### [tedori_hw5p1a.cpp](tedori_hw5p1a.cpp)
This program reads the file listing the edges of authors. The program reads the file and generates the row index array (inline with [CSR](https://en.wikipedia.org/wiki/Sparse_matrix#Compressed_sparse_row_(CSR,_CRS_or_Yale_format)))
and counts the number of co-authors each author has.

### [tedori_hw5p1b.cpp](tedori_hw5p1b.cpp)
This program builds off of an older version of [tedori_hw5p1a.cpp](tedori_hw5p1a.cpp), as such some of the timings might be very different. Regardless, for the part that matters (that times the distribution generation), timing should be consistent with CUDA versions. These differening methods are to ensure fair comparisons between MPI and CUDA implementations.

This file generates the author distribution with shows how many authors have how many co-authors.

### Misc. files
1. [Makefile](Makefile): This compiles both of the above code files. See [running code](https://github.com/niclad/eece5640/tree/master/hw5/p1#running-code) to determine how to compile each file.
2. [dblp-co-authors.txt](dblp-co-authors.txt): This is the author data. See the file for more information.
3. [graphing.py](graphing.py): This generates a graph from the output data. See [author_data.txt](author_data.txt) for the output data and [mpi_dist.pdf](mpi_dist.pdf) for the resultant graph.
4. [mpi_top_co-authors.out](mpi_top_co-authors.out): This is the slurm output for [tedori_hw5p1a.cpp](tedori_hw5p1a.cpp). (I.e., the console output for the program.)
5. [mpi_graph.out](mpi_graph.out): This is the slurm output for [tedori_hw5p1b.cpp](tedori_hw5p1b.cpp). (I.e., the console output for the program.)
6. [p1a.sh](p1a.sh) and [p1b.sh](p1b.sh): These are the `sbatch` script files. See [running code](https://github.com/niclad/eece5640/tree/master/hw5/p1#running-code) for more information.

## Running code
The codefile, `tedori_hw5p1a.cpp` and `tedori_hw5p1b.cpp` can be compiled using `bash$ make` and `bash$ make graph`, respectively. This will generate an executable with the same name as the code file, `tedori_hw5p1<a|b>`.

These programs are run on the Discovery cluster using Slurm. As such, the number of processes is set using `sbatch` or `srun`.

To run using `sbatch`, [p1a.sh](p1a.sh) or [p1b.sh](p1b.sh) can be submitted to run the program in question. An output (`<some_name>.out`) file will be returned once the run is complete. See `*.out` files for examples.

See [Slurm](https://rc-docs.northeastern.edu/en/latest/using-discovery/usingslurm.html) for more information on executing with `srun` or `sbatch`.

## Output
An example output for the command `sbatch p1aa.sh` is below:

```
NUM. PROCESSES: 4
UNIQUE AUTHORS PER PROC: 79270
ELEMENTS PER PROCESS: 79270
Lines processed: 1049866
MPI count time: 0.000541568 sec
MPI topAuth time: 0.000541568 sec
Exe. time for all MPI calls: 0.00108314 sec
Top 10 authors with most co-authors:
AuthID:  3336, co-authors: 343
AuthID:  3345, co-authors: 296
AuthID:   167, co-authors: 290
AuthID: 14690, co-authors: 269
AuthID: 13941, co-authors: 264
AuthID: 30095, co-authors: 244
AuthID: 13842, co-authors: 230
AuthID:   865, co-authors: 227
AuthID:  3298, co-authors: 225
AuthID: 13811, co-authors: 221

```