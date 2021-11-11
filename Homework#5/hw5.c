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
#include <math.h>

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


/*
    msgsize = 4;
    if (allgather(sendbuf,msgsize,MPI_INT,recvbuf,msgsize,MPI_INT,MPI_COMM_WORLD) != 0) {
        printf("The allgather function runs successfully for %d size\n",msgsize);
    }

    
    printf("[%d] ", rank);
    for(j=0;j<msgsize*size;j++) {
        printf("%d ",recvbuf[j]);
    }
    printf("\n");
*/


    for (msgsize = 8; msgsize <= MAXN; msgsize = msgsize << 1) {

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
    int rank, size, i, offset, dest;
    MPI_Status status;
    MPI_Aint lb, sizeofsendtype, sizeofrecvtype;

    if(sendcount != recvcount) {
        printf("Error: sendcount and recvcount are not equal.\n");
        return -1;
    }

    if(sendtype != recvtype) {
        printf("Error: sendtype and recvtype are not equal.\n");
        return -1;
    }

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    
    MPI_Type_get_extent (sendtype, &lb, &sizeofsendtype);
    MPI_Type_get_extent (recvtype, &lb, &sizeofrecvtype);

    //add to local recvbuf
    for(i=0;i<size;i++) {
        offset = i * sendcount * sizeofsendtype;
        MPI_Sendrecv( sendbuf , sendcount , sendtype , i , 0 , 
                    recvbuf+offset , recvcount , recvtype , i , 0 , comm , &status);

    }

    //printf("[%d] 1\n",rank);
    MPI_Barrier(comm);
    //send to other processor
    for(i=0;(int)(pow(2,i))<size;i++) {
        MPI_Barrier(comm);


        if(((rank>>i) & 1) == 0) {
            dest = rank+(int)(pow(2,i));
            //printf("i=[%d] rank=[%d] value = %d dest = %d\n", i, rank, ((rank>>i) & 1),dest);
            offset = dest * sendcount * sizeofsendtype;
            MPI_Sendrecv(sendbuf , sendcount , sendtype , dest , 0, 
                        recvbuf+offset , recvcount , recvtype , dest , 0, comm , &status);

        }
        else {
            dest = rank-(int)(pow(2,i));
            //printf("i=[%d] rank=[%d] value = %d dest = %d\n", i, rank, ((rank>>i) & 1),dest);
            offset = dest * sendcount * sizeofsendtype;
            MPI_Sendrecv(sendbuf ,sendcount , sendtype , dest , 0 , 
                        recvbuf+offset , recvcount , recvtype , dest , 0 , comm , &status);
        }

    }

    
    
    return 0;
}