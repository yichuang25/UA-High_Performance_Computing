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
    int i, rank, size, msgsize, root=0, method;
    int *sendbuf=NULL, *recvbuf=NULL, *myrecvbuf=NULL;
    double t1, t2;

    MPI_Init( &argc, &argv);

    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    if(argc != 2) {
        printf("Usage: %s <method>\n",argv[0]);
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    method = atoi(argv[1]);
    if((method < 1) || (method > 2)) {
        printf("Error, the method not exist\n");
        MPI_Abort( MPI_COMM_WORLD , -1);
    }
    
    sendbuf = (int *) malloc (sizeof (int) * MAXN * size);
    for (i = 0; i < MAXN * size; i++)
	    sendbuf[i] = i;
    

    myrecvbuf = (int *) malloc (sizeof (int) * MAXN);
    recvbuf = (int *) malloc (sizeof(int) * MAXN * size);

#ifdef DEBUG
    int j;
    msgsize = 8;
    if(rank == 0) {
        
        printf("\nWhen msgsize = %d\n",msgsize);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    myscatter (sendbuf, msgsize, MPI_INT, myrecvbuf, msgsize, MPI_INT,
		                 root, MPI_COMM_WORLD);

    if (allgather2(myrecvbuf,msgsize,MPI_INT,recvbuf,msgsize,MPI_INT,MPI_COMM_WORLD) != 0) {
        printf("The allgather function runs successfully for %d size\n",msgsize);
    }

    
    printf("[%d] recvbuf: ", rank);
    for(j=0;j<msgsize*size;j++) {
        printf("%d ",recvbuf[j]);
    }
    printf("\n\n");

    MPI_Barrier(MPI_COMM_WORLD);

#endif

    for(msgsize = 8; msgsize <= MAXN; msgsize = msgsize << 1) {
        myscatter (sendbuf, msgsize, MPI_INT, myrecvbuf, msgsize, MPI_INT, root, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        t1 = MPI_Wtime();

        if(method == 1) {
            allgather1(myrecvbuf,msgsize,MPI_INT,recvbuf,msgsize,MPI_INT,MPI_COMM_WORLD);
        }
        else {
            allgather2(myrecvbuf,msgsize,MPI_INT,recvbuf,msgsize,MPI_INT,MPI_COMM_WORLD);
        }
        
        t2 = MPI_Wtime() - t1;
        MPI_Reduce(&t2, &t1, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        for(i = 0; i<msgsize*size; i++) {
            assert(recvbuf[i] == sendbuf[i]);
        }

        if(rank == 0) {
            printf("Message size = %ld bytes, Maximum time = %g\n",msgsize * sizeof(int), t1/100);
        }

    }

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