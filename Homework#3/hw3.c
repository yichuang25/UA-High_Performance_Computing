/*
* Name: Yichen Huang
* Email: yhuang87@crimson.ua.edu
* Course Section: CS 481
* Homework#: 3
* Instruction to Compile: gcc -Wall -fopenmp -o hw3 hw3.c
* Instruction to Execute: 	./hw3 <size> <max_gen> <num_threads> <output_dir>
           					./life 1000 1000 1 .                   (on your local system)
           					./life 5000 5000 1 /scratch/$USER/     (on DMC at ASC)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

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

/* write array to a file (including ghost cells) */
void writefile(int **a, int N, FILE *fptr) {
  	int i, j;
  	for (i = 0; i < N+2; i++) {
    	for (j = 0; j< N+2; j++)
      		fprintf(fptr, "%d ", a[i][j]);
    	fprintf(fptr, "\n");
  	}
}



int main(int argc, char* argv[]) {
    int N, Max_Gen, Nthreads, **life=NULL, **temp=NULL, **ptr;
    char filename[BUFSIZ];
    FILE *fptr;

    if (argc != 5) {
      	printf("Usage: %s <size> <max_gen> <nthreads> <output dir>\n",argv[0]);
      	exit(-1);
    }

    N = atoi(argv[1]);
    Max_Gen = atoi(argv[2]);
    Nthreads = atoi(argv[3]);
    sprintf(filename, "%s/output.%d.%d.%d",argv[4], N, Max_Gen, Nthreads);
    if((fptr = fopen(filename, "w")) == NULL) {
      	printf("Error opening file %s for writing\n",argv[3]);
      	perror("fopen");
      	exit(-1);
    }

    // Memory Allocation
    life = allocarray(N+2,N+2);
    temp = allocarray(N+2,N+2);

    //initialize boundries of matrix
    for(int i=0;i<N+2;i++) {
      	life[0][i] = life[i][0] = life[N+1][i] = life[i][N+1] = DIES;
      	temp[0][i] = temp[i][0] = temp[N+1][i] = temp[i][N+1] = DIES;
    }

    //randomlize matrix
    for(int i=1;i<N+1;i++) {
      	srand48(54321|i);
      	for(int j=1;j<N+1;j++) {
        	if(drand48()<0.5) {
          		life[i][j] = ALIVE;
        	}
       		 else {
          		life[i][j] = DIES;
        	}
      	}
    }

	int flag=1;
    int j = 0;
	int r,c, value;
	int count = 0;
	int gen = 0;

	//printf("In Gen# %d\n",j);
	//printArray(life,N+2);

    double t1 = gettime();
    //start running
	#pragma omp parallel num_threads(Nthreads) default(none) shared(temp, life, N, Max_Gen, flag, gen) private(j,ptr, r,c,value,count)
    for(j=0;j<Max_Gen; j++) {
      	//flag = compute(life,temp,N);
		
		
		#pragma omp critical
		flag = 0;
		count = 0;
		
		#pragma omp for
		for (r=1;r<N+1;r++) {
			for (c=1;c<N+1;c++) {
				value = life[r-1][c-1] + life[r-1][c] + life[r-1][c+1]
						+ life[r][c-1] + life[r][c+1]
						+ life[r+1][c-1] + life[r+1][c] + life[r+1][c+1];

				if(life[r][c]) {
					if(value < 2 || value > 3) {
						temp[r][c] = DIES;
						count++;
					}
					else {
						temp[r][c] = ALIVE;
					}
				}
				else {
					if(value == 3) {
						temp[r][c] = ALIVE;
						count++;
					}
					else {
						temp[r][c] = DIES;
					}
				}
			}
		}
		
		
		
		#pragma omp critical
		flag += count;

		
		
		
		#pragma omp barrier
		
		#pragma omp single
		{
			ptr = life;
      		life = temp;
      		temp = ptr;
			//printf("flag = %d\n",flag);
			//printf("In Gen# %d\n",j+1);
			//printArray(life,N+2);
			if(flag == 0) {
				gen = j;
			}
		}

		#pragma omp barrier
		{
			//printf("\nflag=%d in thread%d\n",flag,omp_get_thread_num());
			if (flag == 0) {
				j = Max_Gen;
			}
		}
		
		
		#pragma omp barrier 
		
		

	}
		

  	double t2 = gettime();
  	printf("Time taken %f seconds for %d iterations\n", t2 - t1, gen);

	//printArray(life,N+2);

  	/* Write the final array to output file */
  	printf("Writing output to file: %s\n", filename);
  	writefile(life, N, fptr);
  	fclose(fptr);

  	freearray(life);
  	freearray(temp);

  	printf("Program terminates normally\n") ;

    

    return 0;
}