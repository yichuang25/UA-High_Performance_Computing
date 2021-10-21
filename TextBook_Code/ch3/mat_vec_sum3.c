#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char*argv[]) {
    double *x = NULL, *y, sum, mysum, mycount;
    int N, i, myN, rank, size, remain, *counts = NULL, *displays=NULL;
    
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

    if(rank == 0) {
        x = malloc(sizeof(double)*N);  //together
        for(i=0;i<N;i++) {
            x[i] = i + 1.0;
        }
    }
   
    y = malloc(sizeof(double)*myN); //little pieces
    
    counts = malloc(sizeof(double)*size);
    displays = malloc(sizeof(double)*size);
    
    /*
    *
    *   N = 15 , P = 4
    *   myN = 3, remain = 3
    *   4 4 4 3
    */
    for(i=0;i<size;i++) 
        counts[i] = myN + ((i < remain)?1:0);
    
    mycount = counts[rank];

    displays[0] = 0;
    for(i=1;i<size;i++) 
        displays[i] = displays[i-1] + counts[i-1];

    if(rank ==0) {
        for(i=0;i<size;i++) {
            printf("Counts[%d] = %d, display[%d] = %d\n",i,counts[i],i,displays[i]);
        }
    }

#ifdef DEBUG
    MPI_Scatter(x, myN, MPI_DOUBLE, y, myN, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    mysum =0.0;
    for(i=0;i<myN;i++) {
        mysum += y[i];
    }

    MPI_Allreduce(&mysum, &sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    printf("Sum = %f\n",sum);
#endif

    MPI_Scatterv(x, counts, displays, MPI_DOUBLE, y, counts[rank], MPI_DOUBLE,0 ,MPI_COMM_WORLD);
    mysum =0.0;
    for(i=0;i<mycount;i++) {
        mysum += y[i];
    }
    MPI_Allreduce(&mysum, &sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    printf("Sum = %f\n",sum);
    if (rank == 0) {
        free(x);
        free(counts);
        free(displays);
    }
    free(y);
    

    MPI_Finalize();

    return 0;
}