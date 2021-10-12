#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char*argv[]) {
    double *x, sum, mysum;
    int N, i, myN, rank, size, startvalue;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if(argc!=2) {
        printf("Usage: %s <N>\n",argv[0]);
        exit(0);
    }
    N = atoi(argv[1]);
    myN = N/size;
    x = malloc(sizeof(double)*myN);
    startvalue = rank * myN;
    for(i=0;i<N;i++) {
        x[i] =  startvalue + i + 1.0;
    }

    mysum =0.0;
    for(i=0;i<myN;i++) {
        mysum += x[i];
    }
    MPI_Reduce(&mysum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0)
        printf("Sum = %f\n",sum);
    MPI_Finalize();

    return 0;
}