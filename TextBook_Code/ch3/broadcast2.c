#include <stdio.h>
#include <string.h>  /* For strlen             */
#include <stdlib.h>
#include <mpi.h> 

int main(int argc, char* argv[]) {
    int i, rank, size, num;
    MPI_Request request[32];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(rank == 0) {
        num = 12345;
        for(i=1;i<size;i++) {
            MPI_Isend(&num, 1, MPI_INT, i , 0, MPI_COMM_WORLD,&request[i-1]);
        }
        MPI_Waitall(size-1,request, MPI_STATUSES_IGNORE);
    }
    else {
        //MPI_Irecv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request[0]);
        //MPI_Wait(&request[0],MPI_STATUS_IGNORE);
        MPI_Recv(&num, 1, MPI_INT,0,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    printf("rank = %d, num = %d\n",rank, num);

    MPI_Finalize();
    return 0;
}