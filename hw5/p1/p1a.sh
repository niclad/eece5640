#!/bin/bash
#
#SBATCH --export=ALL
#SBATCH --partition=short	
#SBATCH --nodes=2
#SBATCH --tasks-per-node=2
#SBATCH --output=mpi_top_co-authors.out
#SBATCH --time=5:00

mpirun --mca btl_base_warn_component_unused 0 ./mpiAuthors
