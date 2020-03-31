# MPI Author Statistics
Written by Nicolas Tedori. Due April 03, 2020 at 0800.

These programs, written in C++, use MPI to read a [file](dblp-co-authors.txt) containing author information and determine the authors with the most co-authors and graph the distribution of authors with a number of co-authors.

## Files
The files in this directory

### [tedori_hw5p1.cpp](tedori_hw5p1.cpp)
This program reads the file listing the edges of authors. The program reads the file and generates the row index array (inline with [CSR](https://en.wikipedia.org/wiki/Sparse_matrix#Compressed_sparse_row_(CSR,_CRS_or_Yale_format))) and counts the number of co-authors each author has. The file then generates the author distribution which shows how many authors have how many co-authors.

### [Old](old)
This directory contains files from a previous implementation. These files were not removed so they could be referenced in the future, if need be.

### Misc. files
1. [Makefile](Makefile): This compiles the above code files. See [running code](https://github.com/niclad/eece5640/tree/master/hw5/p1#running-code) for more information.
2. [dblp-co-authors.txt](dblp-co-authors.txt): This is the author data. See the file for more information.
3. [graphing.py](graphing.py): This generates a graph from the output data. See [author_data.txt](author_data.txt) for the output data and [mpi_dist.pdf](mpi_dist.pdf) for the resultant graph.
4. [mpi_co-authors.out](mpi_top_co-authors.out): This is the slurm output for [tedori_hw5p1.cpp](tedori_hw5p1.cpp). (I.e., the console output for the program.)
6. [p1.sh](p1.sh): This is the `sbatch` script file. See [running code](https://github.com/niclad/eece5640/tree/master/hw5/p1#running-code) for more information.

## Running code
Below are the steps to compile and run this code on NEU's Discovery Cluster.

### Compiling
The codefile, `tedori_hw5p1.cpp`, can be compiled using `bash$ make`. This will generate an executable with the same name as the code file, `tedori_hw5p1`.

### Running
This program is run on the Discovery cluster using Slurm. As such, the number of processes is set using `sbatch` or `srun`.

To run using `sbatch`, [p1.sh](p1.sh) can be submitted to run the program in question. An output (`mpi_co-authors.out`) file will be returned once the run is complete. See `*.out` files for examples.

See [Slurm](https://rc-docs.northeastern.edu/en/latest/using-discovery/usingslurm.html) for more information on executing with `srun` or `sbatch`.

## Output
An example output for the command `sbatch p1.sh` is below:

```
NUM. PROCESSES: 4
UNIQUE AUTHORS PER PROC: 79270
In-file lines processed: 1049866
MPI count time: 0.000440378 sec
MPI topAuth time: 0.0024089 sec
MPI volume time: 0.000485934 sec
***************************
FINAL RESULTS:
Exe. time for all MPI calls: 0.00333522 sec
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
first 10 dists
43181
58856
49659
35130
25544
18378
13704
10375
8371
6601
Data saved as "author_data.txt"
```