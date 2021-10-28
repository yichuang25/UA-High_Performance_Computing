#!/bin/bash
module load openmpi/4.0.5-gnu-pmi2
./life 1000 5000 1 /scratch/$USER
srun --mpi=pmi2 ./hw4 1000 5000 1 /scratch/$USER
srun --mpi=pmi2 ./hw4 1000 5000 2 /scratch/$USER
srun --mpi=pmi2 ./hw4 1000 5000 4 /scratch/$USER
srun --mpi=pmi2 ./hw4 1000 5000 8 /scratch/$USER
srun --mpi=pmi2 ./hw4 1000 5000 10 /scratch/$USER
srun --mpi=pmi2 ./hw4 1000 5000 16 /scratch/$USER
srun --mpi=pmi2 ./hw4 1000 5000 20 /scratch/$USER