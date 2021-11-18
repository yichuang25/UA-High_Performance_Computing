#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <assert.h>
#include <math.h>
#include "allgather2.h"



int allgather(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm) {
    int rank, size, i, offset, dest, denom, recvoffset, sendoffset;
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
    
    //for(i=0;i<size;i++) {
    offset = rank * sendcount * sizeofsendtype;
    MPI_Sendrecv( sendbuf , sendcount , sendtype , rank , 0 , 
                recvbuf+offset , recvcount , recvtype , rank , 0 , comm , &status);

    //}
    
    
    
    //MPI_Barrier(comm);
    
    /* send to other processor */
    for(i=0;(int)(pow(2,i))<size;i++) {
        MPI_Barrier(comm);

        denom = (int)(pow(2,i));
        if(((rank>>i) & 0b1) == 0) {
            dest = rank+denom;
            
            if(i >= 1) {
                sendoffset = ((rank/denom)*denom) * sendcount  * sizeofsendtype;
                recvoffset = ((dest/denom)*denom) * recvcount  * sizeofrecvtype;
            }
            else {
                sendoffset = rank * sendcount * sizeofsendtype;
                recvoffset = dest * sendcount * sizeofrecvtype;
            }
            //printf("i=[%d] rank=[%d] value = %d dest = %d sendoffset=%d recvoffset=%d\n", i, rank, ((rank>>i) & 1),dest, sendoffset, recvoffset);
            MPI_Sendrecv(recvbuf+sendoffset , sendcount * denom , sendtype , dest , 0, 
                        recvbuf+recvoffset , recvcount * denom , recvtype , dest , 0, comm , &status);

        }
        else {
            dest = rank-denom;
            if(i >= 1) {
                sendoffset = ((rank/denom)*denom) * sendcount  * sizeofsendtype;
                recvoffset = ((dest/denom)*denom) * recvcount  * sizeofrecvtype;
            }
            else {
                sendoffset = rank * sendcount * sizeofsendtype;
                recvoffset = dest * sendcount * sizeofrecvtype;
            }
            //printf("i=[%d] rank=[%d] value = %d dest = %d sendoffset=%d recvoffset=%d\n", i, rank, ((rank>>i) & 1),dest, sendoffset, recvoffset);
            MPI_Sendrecv(recvbuf+sendoffset ,sendcount * denom , sendtype , dest , 0 , 
                        recvbuf+recvoffset , recvcount * denom , recvtype , dest , 0 , comm , &status);
        }

    }
    
  
    return 0;
}