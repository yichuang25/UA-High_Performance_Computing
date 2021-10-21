/* Sample MPI Program that computes the sum of a vector.
 * This version uses MPI_Scatterv function to distribute the data 
 * that is initialized in the process with rank 0. 
 * This program also illustrate the use of MPI_Allreduce function.
 *
 * To Compile: mpicc -O -Wall -o mpi_vec_sum3 mpi_vec_sum3.c 
 * To Run: mpiexec -n 4 ./mpi_vec_sum3 100000 (on your local system)
 *         To run on DMC cluster follow instructions in README.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
	double *x=NULL, *y, sum, mysum;
	int N, i, myN, rank, size, remain, mycount, *counts=NULL, *displs=NULL;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (argc != 2) {
		printf("Usage: %s <N>\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD,-1);
	}
	N = atoi(argv[1]);
	myN = N / size;
	remain = N % size;

	if (rank == 0) {
	    x = malloc(sizeof(double)*N);
	    for (i = 0; i < N; i++)
		    x[i] = i + 1.0;

	    counts = malloc(sizeof(int)*size);
	    displs = malloc(sizeof(int)*size);
	    
	    for (i = 0; i < size; i++)
	      counts[i] = myN + ((i < remain)?1:0);
	    displs[0] = 0;
	    for (i = 1; i < size; i++)
	      displs[i] = displs[i-1] + counts[i-1];
	}
	mycount = myN + ((rank < remain)?1:0);
	y = malloc(sizeof(double)*mycount);

#ifdef DEBUG
	if (rank == 0) {
	   for (i = 0; i < size; i++)
		   printf("counts[%d] = %d, displs[%d] = %d\n", i, counts[i], i, displs[i]);
	} else {
	  printf("rank = %d, mycount = %d\n", rank, mycount);
	}
#endif

	MPI_Scatterv(x, counts, displs, MPI_DOUBLE, y, mycount, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	mysum = 0.0;
	for (i = 0; i < mycount; i++)
		mysum += y[i];

	MPI_Allreduce(&mysum, &sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	printf("[%d]Sum = %f\n", rank, sum);
	if (rank == 0) {
	  free(x);
	  free(counts);
	  free(displs);
	}
	free(y);

	MPI_Finalize();
	return 0;
}