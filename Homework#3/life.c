/* 
   Sample Solution to the game of life program.
   Author: Purushotham Bangalore
   Date: Feb 17, 2010

   Use -DDEBUG1 for output at the start and end.
   Use -DDEBUG2 for output at each iteration.

   To compile: gcc -Wall -O -o life life.c
   To run: ./life <problem size> <max iterations> <nthread> <output dir>
           ./life 1000 1000 1 .                   (on your local system)
           ./life 5000 5000 1 /scratch/$USER/     (on DMC at ASC)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define DIES   0
#define ALIVE  1

/* function to measure time taken */
double gettime(void) {
  struct timeval tval;

  gettimeofday(&tval, NULL);

  return( (double)tval.tv_sec + (double)tval.tv_usec/1000000.0 );
}

/* allocate row-major two-dimensional array */
int **allocarray(int P, int Q) {
  int i, *p, **a;

  p = (int *)malloc(P*Q*sizeof(int));
  a = (int **)malloc(P*sizeof(int*));
  for (i = 0; i < P; i++)
    a[i] = &p[i*Q]; 

  return a;
}

/* free allocated memory */
void freearray(int **a) {
  free(&a[0][0]);
  free(a);
}

/* print arrays in 2D format */
void printarray(int **a, int N, int k) {
  int i, j;
  printf("Life after %d iterations:\n", k) ;
  for (i = 1; i < N+1; i++) {
    for (j = 1; j< N+1; j++)
      printf("%d ", a[i][j]);
    printf("\n");
  }
  printf("\n");
}

/* write array to a file (including ghost cells) */
void writefile(int **a, int N, FILE *fptr) {
  int i, j;
  for (i = 0; i < N+2; i++) {
    for (j = 0; j< N+2; j++)
      fprintf(fptr, "%d ", a[i][j]);
    fprintf(fptr, "\n");
  }
}

/* update each cell based on old values */
int compute(int **life, int **temp, int N) {
  int i, j, value, flag=0;

  for (i = 1; i < N+1; i++) {
    for (j = 1; j < N+1; j++) {
      /* find out the value of the current cell */
      value = life[i-1][j-1] + life[i-1][j] + life[i-1][j+1]
	    + life[i][j-1] + life[i][j+1]
	    + life[i+1][j-1] + life[i+1][j] + life[i+1][j+1] ;
      
      /* check if the cell dies or life is born */
      if (life[i][j]) { // cell was alive in the earlier iteration
	      if (value < 2 || value > 3) {
	        temp[i][j] = DIES ;
	        flag++; // value changed 
	      }
	      else // value must be 2 or 3, so no need to check explicitly
	        temp[i][j] = ALIVE ; // no change
      } 
      else { // cell was dead in the earlier iteration
	      if (value == 3) {
	        temp[i][j] = ALIVE;
	        flag++; // value changed 
	      }
	      else
	        temp[i][j] = DIES; // no change
      }
    }
  }

  return flag;
}


int main(int argc, char **argv) {
  int N, NTIMES, NTHREADS, **life=NULL, **temp=NULL, **ptr ;
  int i, j, k, flag=1;
  double t1, t2;
  char filename[BUFSIZ];
  FILE *fptr;

  if (argc != 5) {
    printf("Usage: %s <size> <max. iterations> <nthreads> <output dir>\n", argv[0]);
    exit(-1);
  }

  N = atoi(argv[1]);
  NTIMES = atoi(argv[2]);
  NTHREADS = atoi(argv[3]);
  sprintf(filename,"%s/output2.%d.%d.%d", argv[4], N, NTIMES, NTHREADS);
  if ((fptr = fopen(filename, "w")) == NULL) {
     printf("Error opening file %s for writing\n", argv[3]);
     perror("fopen");
     exit(-1);
  }

  /* Allocate memory for both arrays */
  life = allocarray(N+2,N+2);
  temp = allocarray(N+2,N+2);

  /* Initialize the boundaries of the life matrix */
  for (i = 0; i < N+2; i++) {
    life[0][i] = life[i][0] = life[N+1][i] = life[i][N+1] = DIES ;
    temp[0][i] = temp[i][0] = temp[N+1][i] = temp[i][N+1] = DIES ;
  }

  /* Initialize the life matrix */
  for (i = 1; i < N+1; i++) {
    srand48(54321|i);
    for (j = 1; j< N+1; j++)
      if (drand48() < 0.5) 
	life[i][j] = ALIVE ;
      else
	life[i][j] = DIES ;
  }

#ifdef DEBUG1
  /* Display the initialized life matrix */
  printarray(life, N, 0);
#endif
  k=0;
	printarray(life,N,k);

  t1 = gettime();
  /* Play the game of life for given number of iterations */
  for (k = 0; k < NTIMES && flag != 0; k++) {
    flag = 0;
    flag = compute(life, temp, N);

    /* copy the new values to the old array */
    ptr = life;
    life = temp;
    temp = ptr;
    printarray(life,N,k);

#ifdef DEBUG2
    /* Print no. of cells alive after the current iteration */
    printf("No. of cells whose value changed in iteration %d = %d\n",k+1,flag) ;

    /* Display the life matrix */
    printarray(life, N, k+1);
#endif
  }
  t2 = gettime();

#ifdef DEBUG1
  /* Display the life matrix after k iterations */
  printarray(life, N, k);
#endif

  printf("Time taken %f seconds for %d iterations\n", t2 - t1, k);

  /* Write the final array to output file */
  printf("Writing output to file: %s\n", filename);
  writefile(life, N, fptr);
  fclose(fptr);

  freearray(life);
  freearray(temp);

  printf("Program terminates normally\n") ;

  return 0;
}

