# CUDA Author Statistics
Written by Nicolas Tedori. Due April 03, 2020 at 0800.

These programs, written in C++ and CUDA, use MPI to read a [file](dblp-co-authors.txt) containing author information and determine the authors with the most co-authors and graph the distribution of authors with a number of co-authors.

## Files
The files in this directory

### [tedori_hw5p2.cu](tedori_hw5p1.cu)
This program reads the file listing the edges of authors. The program reads the file and generates the row index array (inline with [CSR](https://en.wikipedia.org/wiki/Sparse_matrix#Compressed_sparse_row_(CSR,_CRS_or_Yale_format))) and counts the number of co-authors each author has. The file then generates the author distribution which shows how many authors have how many co-authors. The operations used to perform these calculations are the same (or as close as I could get) to those in [p1](../p1).

### [Old](old)
This directory contains files from a previous implementation. These files were not removed so they could be referenced in the future, if need be.

### Misc. files
1. [Makefile](Makefile): This compiles the above code files. See [running code](https://github.com/niclad/eece5640/tree/master/hw5/p1#running-code) for more information.
2. [dblp-co-authors.txt](dblp-co-authors.txt): This is the author data. See the file for more information.
3. [cuda_graphing.py](cuda_graphing.py): This generates a graph from the output data. See [author_data-cuda.txt](author_data-cuda.txt) for the output data and [cuda_dist.pdf](cuda_dist.pdf) for the resultant graph.
4. [cuda_co-authors.out](cuda_co-authors.out): This is the slurm output for [tedori_hw5p2.cu](tedori_hw5p2.cu). (I.e., the console output for the program.)
6. [p2.sh](p2.sh): This is the `sbatch` script files. See [running code](https://github.com/niclad/eece5640/tree/master/hw5/p2#running-code) for more information.

## Running code
Below are the steps to compile and run this code on NEU's Discovery Cluster.

### Compiling
The codefile, `tedori_hw5p2.ccu`, can be compiled using `bash$ make`. This will generate an executable with the same name as the code file, `tedori_hw5p2`. Note that to compile these files, `nvcc` must be installed which requires CUDA. These files were compiled with CUDA 9.0.

### Running
This programs are run on the Discovery cluster using Slurm. As such, the number of processes is set using `sbatch` or `srun`.

To run using `sbatch`, [p1.sh](p1.sh) can be submitted to run the program in question. An output (`cuda_co-authors.out`) file will be returned once the run is complete. See `*.out` files for examples.

See [Slurm](https://rc-docs.northeastern.edu/en/latest/using-discovery/usingslurm.html) for more information on executing with `srun` or `sbatch`.

## Output
An example output for the command `sbatch p2.sh` is below:

```
GPU: Tesla V100-PCIE-32GB: 7.0
Lines processed: 1049866
File read time: 7.59053 sec
GPU author count time: 0.000185429 sec
Max co-auth. search time: 0.0436158 sec
GPU data generation time: 7.09591e-05 sec
***************************
FINAL RESULTS:
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
First 10 distribution values:
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
Last: 1
Data saved as "author_data-cuda.txt"
Cumulative running time: 7.6344 sec
```