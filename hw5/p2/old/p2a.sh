#!/bin/bash
#
#SBATCH --nodes=1
#SBATCH --partition=gpu	
#SBATCH --gres=gpu:k80:1
#SBATCH --output=cuda_top_co-authors.out
#SBATCH --time=5:00

./cudaAuthors
