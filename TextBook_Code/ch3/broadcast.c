#include <stdio.h>
#include <string.h>  /* For strlen             */
#include <stdlib.h>
#include <mpi.h> 

int main(int argc, char* argv[]) {
    int i, rank, size, num;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(rank == 0) {
        num = 12345;
        for(i=0;i<size;i++) {
            MPI_Send(&num, 1, MPI_INT, i , 0, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    printf("rank = %d, num = %d\n",rank, num);

    MPI_Finalize();
    return 0;
}