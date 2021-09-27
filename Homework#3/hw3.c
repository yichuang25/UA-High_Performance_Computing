/*
* Name: Yichen Huang
* Email: yhuang87@crimson.ua.edu
* Course Section: CS 481
* Homework#: 1
* Instruction to Compile: gcc -Wall -fopenmp -o hw3 hw3.c
* Instruction to Execute: ./hw3 <size> <max_gen> <num_threads> <output_dir>
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define DIES   0
#define ALIVE  1

//print the array with 0(dead) and 1 (alive)
static void printArray(int **array, int size) { 
    for(int i=1;i<size-1;i++) {
        for(int j=1;j<size-1;j++) {
            printf("%d ", array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

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

void randomlize(int **array, int size) { // randomlize each cell in the graph
    srand(1);
    for(int i=1;i<size-1;i++) {
        for(int j=1;j<size-1;j++) {
            array[i][j] = rand() % 2;
        }
    }
}



int main(int argc, char* argv[]) {
    char filename[BUFSIZ];
    FILE *fptr;

    

    return 0;
}