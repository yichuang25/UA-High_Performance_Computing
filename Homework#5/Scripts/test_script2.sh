#!/bin/bash
module load openmpi/4.0.5-gnu-pmi2
srun --mpi=pmi2 ./myallgather1
srun --mpi=pmi2 ./myallgather2