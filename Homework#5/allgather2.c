#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <assert.h>
#include <math.h>

int allgather(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);

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

    /* send to local recvbuf */
    for(i=0;i<size;i++) {
        offset = i * sendcount * sizeofsendtype;
        MPI_Sendrecv( sendbuf , sendcount , sendtype , i , 0 , 
                    recvbuf+offset , recvcount , recvtype , i , 0 , comm , &status);

    }

    //printf("[%d] 1\n",rank);
    MPI_Barrier(comm);
    
    /* send to other processor */
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