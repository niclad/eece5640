#!/bin/bash
#
#SBATCH --export=ALL
#SBATCH --partition=short	
#SBATCH --nodes=2
#SBATCH --tasks-per-node=1
#SBATCH --output=test.out
#SBATCH --time=7:30

mpirun --mca btl_base_warn_component_unused 0 ./binning 10000000 2 10
