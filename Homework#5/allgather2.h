#ifndef __ALLGATHER2_H__
#define __ALLGATHER2_H__

/**
 * @brief allgather function using butterfly structure
 * 
 * @param sendbuf pointer of the sendbuffer
 * @param sendcount size of the sent message
 * @param sendtype  data type of sent message
 * @param recvbuf  pointer of the receivebuffer
 * @param recvcount size of the received message
 * @param recvtype data type of received message
 * @param comm MPI_COMM_WORLD
 * @return if success return 0; otherwise return -1
 */
int allgather(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);

#endif