#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char*argv[]) {
    double *x = NULL, *y, sum, mysum;
    int N, i, myN, rank, size, remain;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    //command line argument check
    if(argc!=2) {
        printf("Usage: %s <N>\n",argv[0]);
        exit(0);
    }

    N = atoi(argv[1]);
    myN = N/size;
    remain = N % size;

    /*
    *
    *   N = 15 , P = 4
    *   myN = 3, remain = 3
    *   4 4 4 3
    */

    if(rank<remain) {  // make sure even
        myN++;
    }

    if(rank == 0) {
        x = malloc(sizeof(double)*N);  //together
        for(i=0;i<N;i++) {
            x[i] = i + 1.0;
        }
    }
   
    y = malloc(sizeof(double)*myN); //little pieces
    

    
    MPI_Scatter(x, myN, MPI_DOUBLE, y, myN, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    mysum =0.0;
    for(i=0;i<myN;i++) {
        mysum += y[i];
    }

    MPI_Reduce(&mysum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    printf("Sum = %f\n",sum);
    if (rank == 0) {
        
        free(x);
    }
    free(y);

    MPI_Finalize();

    return 0;
}