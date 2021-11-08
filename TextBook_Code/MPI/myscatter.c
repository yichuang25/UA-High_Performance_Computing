/* Sample program to illustrate the scatter operation using
 * non-blocking point-to-point primitives.  
 *
 * To Compile: mpicc -Wall -o myscatter myscatter.c
 * To Run: On your local system: mpiexec -n 4 ./myscatter 
 *         On DMC cluster, see instructions provided in README.md
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

#define MAXN 262144
#define NTIMES 100

void myscatter (void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, int recvcount, MPI_Datatype recvtype,
		int root, MPI_Comm comm);

int main (int argc, char **argv) {
  int i, rank, size, msgsize, root = 0, offset;
  int *sendbuf = NULL, *recvbuf;
  double t1, t2;

  MPI_Init (&argc, &argv);

  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &size);

  if (rank == root) {
      sendbuf = (int *) malloc (sizeof (int) * MAXN * size);
      for (i = 0; i < MAXN * size; i++)
	        sendbuf[i] = i;
  }
  recvbuf = (int *) malloc (sizeof (int) * MAXN);

  for (msgsize = 8; msgsize <= MAXN; msgsize = msgsize << 1) {
      /* setup a synchronization point */
      MPI_Barrier (MPI_COMM_WORLD);
      t1 = MPI_Wtime ();

      /* include rest of the program here */
      for (i = 0; i < NTIMES; i++)
	        myscatter (sendbuf, msgsize, MPI_INT, recvbuf, msgsize, MPI_INT,
		                 root, MPI_COMM_WORLD);

      /* program end here */
      t2 = MPI_Wtime () - t1;

      MPI_Reduce (&t2, &t1, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

      /* check if everyone got the correct results */
      for (i = 0, offset = rank * msgsize; i < msgsize; i++)
	        assert (recvbuf[i] == (offset + i));

      if (rank == 0) {
	       printf ("Message size = %ld bytes, Maximum time = %g\n",
		             msgsize * sizeof (int), t1 / 100.0);
      }
  }

  MPI_Finalize ();

  return 0;
}

void myscatter (void *sendbuf, int sendcount, MPI_Datatype sendtype,
	        void *recvbuf, int recvcount, MPI_Datatype recvtype,
	        int root, MPI_Comm comm) {
  int rank, size, i, offset;
  MPI_Status *status;
  MPI_Request *request;
  MPI_Aint lb, sizeofsendtype, sizeofrecvtype;

  MPI_Comm_rank (comm, &rank);
  MPI_Comm_size (comm, &size);

  status = malloc (sizeof (MPI_Status) * (size + 1));
  request = malloc (sizeof (MPI_Request) * (size + 1));

  if (rank == root) {	/* If I'm the root node, send it to others */
      for (i = 0; i < size; i++) {
	        MPI_Type_get_extent (sendtype, &lb, &sizeofsendtype);
	        offset = sizeofsendtype * sendcount * i;
	        char *bufptr = sendbuf + offset;
	        MPI_Isend (bufptr, sizeofsendtype * sendcount, MPI_CHAR, i, 0, comm,
		                 &request[i]);
      }
      MPI_Type_get_extent (recvtype, &lb, &sizeofrecvtype);
      MPI_Irecv (recvbuf, sizeofrecvtype * recvcount, MPI_CHAR, root, 0, comm,
		             &request[i]);
      MPI_Waitall (size + 1, request, status);
  } else {    /* all other processes receive from process with rank root */
      MPI_Type_get_extent (recvtype, &lb, &sizeofrecvtype);
      MPI_Irecv (recvbuf, sizeofrecvtype * recvcount, MPI_CHAR, root, 0, comm,
		             &request[0]);
      MPI_Wait (&request[0], MPI_STATUS_IGNORE);
  }

  free (request);
  free (status);
}