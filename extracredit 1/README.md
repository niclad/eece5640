# Extra Credit 1 - Dense and Sparse Matrix Multiplication
This code optimizes dense and sparse matrix multiplication. Everything needed to compile and run this code [Linux) is provided in this directory. Follow the information below. All code files are in C.

## Files
1. [M-M_none.c](M-M_none.c): This is defualt matrix-matrix multiplication code without any optimizations applied. This is naive multiplication.

2. [M-M.c](M-M.c): This is the matrix-matrix multiplication code using blocking. 

3. [sparse_reg.c](sparse_reg.c): This is the non-optimized version of the sparse matrix multiplication. Naive multiplication.

4. [M-M-sparse.c](M-M-sparse.c): Optimized sparse matrix multiplication. This is still naive but is optimized by skipping unnecessary multiplications. Not sure if this is an acceptable solution, but I think it's good [I followed the directions).

5. [Makefile](Makefile): Compiles the above code files. All code is compiled with optimization settings `-O0`.
   - `make` compiles [M-M.c](M-M.c), making the output file `m-m`.
   - `make noopt` compiles [M-M_none.c](M-M_none.c), making the output file `m-m_no`.
   - `make sparse` compiles [M-M-sparse.c](M-M-sparse.c), making the output file `sparse`.
   - `make reg-sparse` compiles [sparse_reg.c](sparse_reg.c), making the output file `reg-sparse`.

## Speed-up
These timing were recorded on the COE **Eta** machines.

|**CPU**| **Memory**| **OS**| 
|Intel Xeon CPU X5650 @ 2.67GHz; 6 cores/socket, 2 sockets; 24 hardware threads |L1i/d sizes of 32 KB ea., L2 of 256 KB, L3 of 12.3 MB. 49 GB RAM| CentOS 7.4|

The execution time for non-optimized dense matrix multiplication is ~1191.22ms. The execution time for the optimized dense matrix multiplication ~665.67ms. This makes the speed-up for [M-M.c](M-M.c) ~1.68X.

The execution time for non-optimized sparse matrix multiplication is ~893.86ms. The execution time for the optimized sparse matrix multiplication ~637.08ms. This makes the speed-up for [M-M-sparse.c](M-M.c) ~1.40X.

## Running
Run the files in bash with `$ ./<output_file_name>`. See [Files](https://github.com/niclad/eece5640/tree/master/extracredit%201#files) for more information. Output will show values for multiplication results to confirm success, as well as running time (in milliseconds). No input for the files is required.