# CUDA Author Statistics
Written by Nicolas Tedori. Due April 03, 2020 at 0800.

These programs, written in C++ and CUDA, use MPI to read a [file](dblp-co-authors.txt) containing author information and determine the authors with the most co-authors and graph the distribution of authors with a number of co-authors.

## Files
The files in this directory

### [tedori_hw5p2a.cu](tedori_hw5p1a.cu)
This program reads the file listing the edges of authors. The program reads the file and generates the row index array (inline with [CSR](https://en.wikipedia.org/wiki/Sparse_matrix#Compressed_sparse_row_(CSR,_CRS_or_Yale_format)))
and counts the number of co-authors each author has. The operations in this program are functionally equivalent to those in [p1](../p1).

### [tedori_hw5p2b.cu](tedori_hw5p2b.cu)
This program builds off of the current version of [tedori_hw5p2a.cu](tedori_hw5p2a.cu). This file generates the author distribution with shows how many authors have how many co-authors. The operations in this program are functionally equivalent to those in [p1](../p1).

### Misc. files
1. [Makefile](Makefile): This compiles both of the above code files. See [running code](https://github.com/niclad/eece5640/tree/master/hw5/p2#running-code) to determine how to compile each file.
2. [dblp-co-authors.txt](dblp-co-authors.txt): This is the author data. See the file for more information.
3. [graphing.py](graphing.py): This generates a graph from the output data. See [author_data-cuda.txt](author_data-cuda.txt) for the output data and [cuda_dist.pdf](cuda_dist.pdf) for the resultant graph.
4. [cuda_top_co-authors.out](cuda_top_co-authors.out): This is the slurm output for [tedori_hw5p2a.cu](tedori_hw5p2a.cu). (I.e., the console output for the program.)
5. [cuda_graph.out](cuda_graph.out): This is the slurm output for [tedori_hw5p2b.cu](tedori_hw5p2b.cu). (I.e., the console output for the program.)
6. [p2a.sh](p2a.sh) and [p2b.sh](p2b.sh): These are the `sbatch` script files. See [running code](https://github.com/niclad/eece5640/tree/master/hw5/p2#running-code) for more information.

## Running code
The codefile, `tedori_hw5p2a.cpp` and `tedori_hw5p2b.cpp` can be compiled using `bash$ make` and `bash$ make graph`, respectively. This will generate an executable with the same name as the code file, `tedori_hw5p2<a|b>`. Note that to compile these files, `nvcc` must be installed which requires CUDA. These files were compiled with CUDA 9.0.

These programs are run on the Discovery cluster using Slurm. As such, the number of processes is set using `sbatch` or `srun`.

To run using `sbatch`, [p1a.sh](p1a.sh) or [p1b.sh](p1b.sh) can be submitted to run the program in question. An output (`<some_name>.out`) file will be returned once the run is complete. See `*.out` files for examples.

See [Slurm](https://rc-docs.northeastern.edu/en/latest/using-discovery/usingslurm.html) for more information on executing with `srun` or `sbatch`.

## Output
An example output for the command `sbatch p1b.sh` is below:

```
GPU: Tesla K80: 3.7
Lines processed: 1049866
File read time: 0.847428 sec
GPU author count time: 0.000274836 sec
Max co-auth. search time: 0.00659489 sec
auth  3336, count 343
auth  3345, count 296
auth   167, count 290
auth 14690, count 269
auth 13941, count 264
auth 30095, count 244
auth 13842, count 230
auth   865, count 227
auth  3298, count 225
auth 13811, count 221
Total running time: 0.854298 sec

```