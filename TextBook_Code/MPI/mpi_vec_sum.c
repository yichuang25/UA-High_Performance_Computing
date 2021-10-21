/* Sample MPI Program that computes the sum of a vector.
 * To Compile: mpicc -O -Wall -o mpi_vec_sum mpi_vec_sum.c 
 * To Run: mpiexec -n 4 ./mpi_vec_sum 100000 (on your local system)
 *         To run on DMC cluster follow instructions in README.
 */
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {

        int rank,size,N,i,myN;
        double sum, mysum, *a=NULL, *b=NULL, t1, t2;

        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);

        printf("My rank is %d, size is %d\n", rank, size);

        if (argc != 2) {
           printf("Usage: %s <# of elements>\n",argv[0]);
           MPI_Abort(MPI_COMM_WORLD,-1);
        }

        N = atoi(argv[1]);
        if (rank == 0) {
            a = (double *)malloc(sizeof(double)*N);

            for (i=0; i<N; i++)
                a[i] = (double)(i + 1);
        }

        myN = N/size;	// assume N is divisible by size
        b = (double *)malloc(sizeof(double)*myN);

      	MPI_Barrier(MPI_COMM_WORLD);
      	t1 = MPI_Wtime();

        MPI_Scatter(a, myN, MPI_DOUBLE, b, myN, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        for (mysum=0, i=0; i<myN; i++)
            mysum += b[i];

        MPI_Reduce(&mysum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	      t2 = MPI_Wtime();
        printf("[%d]: Time taken = %f\n", rank, t2-t1);

        if (rank == 0) {
            printf("[%d]: The total is %f it should be equal to %f\n",
                   rank, sum, ((double)N*(N+1))/2);
	          free(a);
	      }
	      free(b);

        MPI_Finalize();

        return 0;
}