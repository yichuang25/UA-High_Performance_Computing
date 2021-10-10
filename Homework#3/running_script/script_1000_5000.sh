#!/bin/bash
module load intel
./life 1000 5000 1 /scratch/$USER
./hw3 1000 5000 1 /scratch/$USER
./hw3 1000 5000 2 /scratch/$USER
./hw3 1000 5000 4 /scratch/$USER
./hw3 1000 5000 8 /scratch/$USER
./hw3 1000 5000 10 /scratch/$USER
./hw3 1000 5000 16 /scratch/$USER
./hw3 1000 5000 20 /scratch/$USER