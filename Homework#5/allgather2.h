#ifndef __ALLGATHER2_H__
#define __ALLGATHER2_H__

int allgather2(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);

#endif