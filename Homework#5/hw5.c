/*
* Name: Yichen Huang
* Email: yhuang87@crimson.ua.edu
* Course Section: CS 481
* Homework#: 5
* Instruction to Compile: mpicc -O -Wall -o hw5 hw5.c
* Instruction to Execute: 	mpiexec -n <num_processors> ./hw5
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <assert.h>

#define MAXN 1048576
#define NTIMES 3

int allgather(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);

int main (int argc, char **argv) {
    int i, rank, size, msgsize, offset;
    int *sendbuf = NULL, *recvbuf=NULL;
    double t1, t2;

    MPI_Init (&argc, &argv);

    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    //inital buffers
    sendbuf = (int *) malloc (sizeof(int) * MAXN);
    for(i=0;i<MAXN;i++) {
        sendbuf[i] = i;
    }
    recvbuf = (int *) malloc (sizeof(int) * MAXN * size);

    


    

    



    MPI_Finalize ();

    return 0;
}