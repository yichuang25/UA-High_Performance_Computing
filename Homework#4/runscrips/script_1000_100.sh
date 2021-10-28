#!/bin/bash
module load openmpi/4.0.5-gnu-pmi2
./life 1000 100 1 /scratch/$USER
srun --mpi=pmi2 ./hw4 1000 100 1 /scratch/$USER
srun --mpi=pmi2 ./hw4 1000 100 2 /scratch/$USER
srun --mpi=pmi2 ./hw4 1000 100 4 /scratch/$USER
srun --mpi=pmi2 ./hw4 1000 100 8 /scratch/$USER
srun --mpi=pmi2 ./hw4 1000 100 10 /scratch/$USER
srun --mpi=pmi2 ./hw4 1000 100 16 /scratch/$USER
srun --mpi=pmi2 ./hw4 1000 100 20 /scratch/$USER