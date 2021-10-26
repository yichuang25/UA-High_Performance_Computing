	#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
	double *x=NULL, *y, sum, mysum;
	int N, i, myN, rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (argc != 2) {
		printf("Usage: %s <N>\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD,-1);
	}
	N = atoi(argv[1]);
	myN = N / size;

	if (rank == 0) {
      /* allocate and initialize only in process with rank 0 */
	    x = malloc(sizeof(double)*N);
	    for (i = 0; i < N; i++)
	 	      x[i] = i + 1.0;
	}
	y = malloc(sizeof(double)*myN);

	MPI_Scatter(x, myN, MPI_DOUBLE, y, myN, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	mysum = 0.0;
	for (i = 0; i < myN; i++)
		mysum += y[i];

	MPI_Allreduce(&mysum, &sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	printf("[%d]Sum = %f\n", rank, sum);
	if (rank == 0) free(x); // NOTE: free x only in rank 0
	free(y);
	MPI_Finalize();
	return 0;
}