#ifndef __ALLGATHER1_H__
#define __ALLGATHER1_H__


int allgather1(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);

#endif