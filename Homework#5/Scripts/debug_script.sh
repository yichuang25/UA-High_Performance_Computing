#!/bin/bash
module load openmpi/4.0.5-gnu-pmi2
srun --mpi=pmi2 ./ver1
srun --mpi=pmi2 ./ver2
