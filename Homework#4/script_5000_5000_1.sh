#!/bin/bash
module load openmpi/4.0.5-gnu-pmi2
./life 5000 5000 1 /scratch/$USER
srun --mpi=pmi2 ./hw4 5000 5000 1 /scratch/$USER
srun --mpi=pmi2 ./hw4 5000 5000 1 /scratch/$USER
srun --mpi=pmi2 ./hw4 5000 5000 1 /scratch/$USER