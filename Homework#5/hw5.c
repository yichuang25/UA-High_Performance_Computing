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

    for (msgsize = 32; msgsize <= MAXN; msgsize = msgsize << 1) {

        MPI_Barrier(MPI_COMM_WORLD);
        t1 = MPI_Wtime();

        allgather(sendbuf,msgsize,MPI_INT,recvbuf,msgsize,MPI_INT,MPI_COMM_WORLD);

        t2 = MPI_Wtime() - t1;
        MPI_Reduce(&t2, &t1, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        //check results
        for(i = 0; i<msgsize*size; i++) {
            offset = i%msgsize;
            assert(recvbuf[i] == offset);
        }

        if(rank == 0) {
            printf("Message size = %ld bytes, Maximum time = %g\n",msgsize * sizeof(int), t1/100);
        }
    }

    MPI_Finalize ();

    return 0;
}

int allgather(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    int rank, size, i, offset;
    MPI_Status *status;
    MPI_Request *request;
    MPI_Aint lb, sizeofsendtype, sizeofrecvtype;

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    status = malloc (sizeof(MPI_Status) * (size + 1));
    request = malloc (sizeof(MPI_Request) * (size + 1));

    

}