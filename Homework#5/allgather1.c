#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <assert.h>
#include <math.h>
#include "allgather1.h"

int allgather1(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    int rank, size, i, offset;
    MPI_Status *status;
    MPI_Request *request;
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

    status = malloc (sizeof (MPI_Status) * (size));
    request = malloc (sizeof (MPI_Request) * (size));

    MPI_Type_get_extent (sendtype, &lb, &sizeofsendtype);
    MPI_Type_get_extent (recvtype, &lb, &sizeofrecvtype);

    //send to root processor
    
    MPI_Type_get_extent (sendtype, &lb, &sizeofsendtype);
	
    MPI_Isend(sendbuf , sendcount , sendtype , 0 , 0 , MPI_COMM_WORLD , &request[rank]);

    MPI_Type_get_extent (recvtype, &lb, &sizeofrecvtype);

    if (rank == 0) {
        MPI_Type_get_extent (recvtype, &lb, &sizeofrecvtype);
        for(i=0;i<size;i++) {
            offset = sizeofsendtype * sendcount * i;
            MPI_Irecv( recvbuf+offset , recvcount , recvtype , i , 0 , MPI_COMM_WORLD , &request[i]);
        }
        MPI_Waitall (size, request, status);
    }

    //broadcast
    if(rank == 0) {
        for(i=1;i<size;i++) {
            MPI_Isend(recvbuf, sendcount*size, sendtype, i, 0, MPI_COMM_WORLD, &request[i-1]);
        }
        MPI_Waitall(size-1, request, MPI_STATUSES_IGNORE);

    }
    else {
        MPI_Recv(recvbuf, recvcount*size , recvtype, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
    }

    
    
    free(request);
    free(status);

    


    return 0;   
}