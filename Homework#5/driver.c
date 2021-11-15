/**
* Name: Yichen Huang
* Email: yhuang87@crimson.ua.edu
* Course Section: CS 481
* Homework#: 5
* Instruction to Compile (with DEBUG):  mpicc -O -Wall -D DEBUG -lm -o ver1 driver.c allgather1.c
*                                       mpicc -O -Wall -D DEBUG -lm -o ver2 driver.c allgather2.c
* Instruction to Compile (without DEBUG):   mpicc -O -Wall -lm -o ver1 driver.c allgather1.c
*                                           mpicc -O -Wall -lm -o ver2 driver.c allgather2.c
* Instruction to Execute: 	mpiexec -n <num_processors> ./ver1
*                           mpiexec -n <num_processors> ./ver2
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <assert.h>
#include <math.h>
#include "allgather1.h"
#include "allgather2.h"

#define MAXN 1048576

void myscatter (void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, int recvcount, MPI_Datatype recvtype,
		int root, MPI_Comm comm);

int main(int argc, char *argv[]) {
    int i, rank, size, msgsize, root=0;
    int *sendbuf=NULL, *recvbuf=NULL, *myrecvbuf=NULL;
    double t1, t2;

    MPI_Init( &argc, &argv);

    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    

    /* allocate send and receive buffers */
    sendbuf = (int *) malloc (sizeof (int) * MAXN * size);
    for (i = 0; i < MAXN * size; i++)
	    sendbuf[i] = i;
    

    myrecvbuf = (int *) malloc (sizeof (int) * MAXN);
    recvbuf = (int *) malloc (sizeof(int) * MAXN * size);

/* BEBUG Micro: printout the recvbuffer after allgather function (Print from 0 to msgsize-1 num_processor times) */
#ifdef DEBUG
    int j;
    msgsize = 8;
    if(rank == 0) {
        
        printf("\nWhen msgsize = %d\n",msgsize);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    myscatter (sendbuf, msgsize, MPI_INT, myrecvbuf, msgsize, MPI_INT,
		                 root, MPI_COMM_WORLD);

    if (allgather(myrecvbuf,msgsize,MPI_INT,recvbuf,msgsize,MPI_INT,MPI_COMM_WORLD) != 0) {
        printf("The allgather function runs successfully for %d size\n",msgsize);
    }

    
    printf("[%d] recvbuf: ", rank);
    for(j=0;j<msgsize*size;j++) {
        printf("%d ",recvbuf[j]);
    }
    printf("\n\n");

    MPI_Barrier(MPI_COMM_WORLD);

#endif

    /*  Test message size from 8 to 1048576   */
    for(msgsize = 8; msgsize <= MAXN; msgsize = msgsize << 1) {
        myscatter (sendbuf, msgsize, MPI_INT, myrecvbuf, msgsize, MPI_INT, root, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        t1 = MPI_Wtime();

        
        allgather(myrecvbuf,msgsize,MPI_INT,recvbuf,msgsize,MPI_INT,MPI_COMM_WORLD);
        
        t2 = MPI_Wtime() - t1;
        /* Get the max time */
        MPI_Reduce(&t2, &t1, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        /* Check recvbuf result*/
        for(i = 0; i<msgsize*size; i++) {
            assert(recvbuf[i] == sendbuf[i]);
        }

        if(rank == 0) {
            printf("Message size = %ld bytes, Maximum time = %g\n",msgsize * sizeof(int), t1/100);
        }

    }

    /* free send and receive buffers */
    free(sendbuf);
    free(recvbuf);
    free(myrecvbuf);
    MPI_Finalize();

    return 0;
}

void myscatter (void *sendbuf, int sendcount, MPI_Datatype sendtype,
	        void *recvbuf, int recvcount, MPI_Datatype recvtype,
	        int root, MPI_Comm comm) {
  int rank, size, i, offset;
  MPI_Status *status;
  MPI_Request *request;
  MPI_Aint lb, sizeofsendtype, sizeofrecvtype;

  MPI_Comm_rank (comm, &rank);
  MPI_Comm_size (comm, &size);

  status = malloc (sizeof (MPI_Status) * (size + 1));
  request = malloc (sizeof (MPI_Request) * (size + 1));

  if (rank == root) {	/* If I'm the root node, send it to others */
      for (i = 0; i < size; i++) {
	        MPI_Type_get_extent (sendtype, &lb, &sizeofsendtype);
	        offset = sizeofsendtype * sendcount * i;
	        char *bufptr = sendbuf + offset;
	        MPI_Isend (bufptr, sizeofsendtype * sendcount, MPI_CHAR, i, 0, comm,
		                 &request[i]);
      }
      MPI_Type_get_extent (recvtype, &lb, &sizeofrecvtype);
      MPI_Irecv (recvbuf, sizeofrecvtype * recvcount, MPI_CHAR, root, 0, comm,
		             &request[i]);
      MPI_Waitall (size + 1, request, status);
  } else {    /* all other processes receive from process with rank root */
      MPI_Type_get_extent (recvtype, &lb, &sizeofrecvtype);
      MPI_Irecv (recvbuf, sizeofrecvtype * recvcount, MPI_CHAR, root, 0, comm,
		             &request[0]);
      MPI_Wait (&request[0], MPI_STATUS_IGNORE);
  }

  free (request);
  free (status);
}