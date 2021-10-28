/*
* Name: Yichen Huang
* Email: yhuang87@crimson.ua.edu
* Course Section: CS 481
* Homework#: 4
* Instruction to Compile: mpicc -O -Wall -o hw4 hw4.c
* Instruction to Execute: 	mpiexec -n <num_processors> ./hw4 <size> <max_gen> <num_processors> <output dir>
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define DIES 0
#define ALIVE 1

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

/* print array with ghost cell*/
void printwhole(int **a, int R, int C, int rank) {
    int i, j;
    printf("[%d] \n",rank);
    for (i = 0; i < R; i++) {
        for (j = 0; j< C; j++)
            printf("%d ", a[i][j]);
        printf("\n");
    }
    printf("\n");
}

/* print arrays in 2D format */
void printarray(int **a, int R, int C, int rank) {
    int i, j;
    printf("[%d] \n",rank);
    for (i = 1; i < R+1; i++) {
        for (j = 1; j< C+1; j++)
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

int main(int argc, char **argv) {
    int N, myN, NTIMES, NCORES, **life=NULL, **temp=NULL, **ptr, **mylife=NULL, **mytemp=NULL;
    int i, j, k, max_iter=0, value, flag=0, myflag=0, size, rank, remain, mycount, *counts=NULL, *displs=NULL;
    double t1, t2;
    char filename[BUFSIZ];
    FILE *fptr = NULL;
    MPI_Status status;
    MPI_Comm comm;


    MPI_Init(&argc, &argv);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm,&rank);

    if (argc != 5) {
        printf("Usage: %s <size> <max_gen> <nprocessors> <output dir>\n",argv[0]);
        MPI_Abort(comm,-1);
    }

    N = atoi(argv[1]);
    myN = N / size;
    remain = N % size;
    NTIMES = atoi(argv[2]);
    NCORES = atoi(argv[3]);
    sprintf(filename,"%s/output.%d.%d.%d",argv[4],N,NTIMES,NCORES);

    int *bufptr=NULL;
    //int *countptr = NULL;
    if(rank == 0) {
        if ((fptr = fopen(filename, "w")) == NULL) {
            printf("Error opening file %s for writing\n", argv[3]);
            perror("fopen");
            MPI_Abort(comm,-1);
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
        bufptr = &life[1][0];

        counts = malloc(sizeof(int)*size);
	    displs = malloc(sizeof(int)*size);

        for (i = 0; i < size; i++) {
            counts[i] = (myN + ((i < remain)?1:0)) * (N+2);
            //printf("%d ", counts[i]);
        }
        //printf("\n");
	      
	    displs[0] = 0;
	    for (i = 1; i < size; i++) {
            displs[i] = displs[i-1] + counts[i-1];
            //printf("%d ", displs[i]);
        }
        //printf("\n");
	    //countptr = counts;
        //printwhole(life,N+2,N+2, rank);
    }

    MPI_Barrier(comm);

    //create local
    mycount = myN + ((rank < remain)?1:0);
    mylife = allocarray(mycount+2,N+2);
    mytemp = allocarray(mycount+2,N+2);
    /* Initialize the boundaries of the mylife matrix */
    for (i = 0; i < mycount+2; i++) {
        for(j=0;j<N+2;j++) {
            if(i==0 || i==mycount+1) {
                mylife[i][j] = DIES;
                mytemp[i][j] = DIES;
            }

            if((j==0) || (j==N+1)) {
                mylife[i][j] = DIES;
                mytemp[i][j] = DIES;
            }
        }
        
    }
    
    /* scatter the life to mylife */
    MPI_Scatterv(bufptr, counts, displs, MPI_INT,mylife[1], mycount*(N+2), MPI_INT, 0, comm);
    //printwhole(mylife,mycount+2,N+2, rank);
    MPI_Barrier(comm);

    /* swap to ghost cells */
    int up = rank-1;
    int down = rank+1;
    
    //printf("[%d] mycount = %d\n",rank,mycount);
    //printf("[%d] up = %d, down = %d\n",rank,up,down);
    MPI_Barrier(comm);

    if(up < 0) {
        if(down < size) {
            MPI_Sendrecv(mylife[mycount], N+2, MPI_INT, down, 0,
                    mylife[mycount+1], N+2, MPI_INT, down, 0, comm, &status);
        }
        
    }
    else if(down >= size) {

        MPI_Sendrecv(mylife[1], N+2, MPI_INT, up, 0,
                    mylife[0], N+2, MPI_INT, up, 0, comm, &status);
    }
    else {
        MPI_Sendrecv(mylife[1], N+2, MPI_INT, up, 0,
                    mylife[0], N+2, MPI_INT, up, 0, comm, &status);
                    
        MPI_Sendrecv(mylife[mycount], N+2, MPI_INT, down, 0,
                    mylife[mycount+1], N+2, MPI_INT, down, 0, comm, &status);
    }

    MPI_Barrier(comm);
    //start computing
    t1 = gettime();

    for(k=0;k<NTIMES;k++) {
        //printf("[%d] k = %d\n",rank,k);
        
        
        value = 0;
        myflag = 0;
        flag = 0;
        MPI_Barrier(comm);


        for(i=1;i<mycount+1;i++) {
            for(j=1;j<N+1;j++) {
                value = mylife[i-1][j-1] + mylife[i-1][j] + mylife[i-1][j+1]
			    	+ mylife[i][j-1] + mylife[i][j+1]
			    	+ mylife[i+1][j-1] + mylife[i+1][j] + mylife[i+1][j+1] ;

                if (mylife[i][j]) { // cell was alive in the earlier iteration
			      	if (value < 2 || value > 3) {
			        	mytemp[i][j] = DIES ;
			        	myflag++; // value changed 
			      	}
			      	else // value must be 2 or 3, so no need to check explicitly
			        	mytemp[i][j] = ALIVE ; // no change
  	  	  	    } 
  	  	  	    else { // cell was dead in the earlier iteration
			      	if (value == 3) {
			        	mytemp[i][j] = ALIVE;
			        	myflag++; // value changed 
			      	}
			      	else
			        	mytemp[i][j] = DIES; // no change
  	  		    }
            }
        }
        //printf("[%d] myflag = %d\n",rank,myflag);

        MPI_Barrier(comm);
        MPI_Allreduce(&myflag, &flag, 1, MPI_INT, MPI_SUM, comm);
        MPI_Barrier(comm);

        //printf("[%d] In gen# %d flag = %d\n",rank,k,flag);
        if(flag!=0) {
            
            //swap ghost
            if(up < 0) { 
                if(down < size) {
                    MPI_Sendrecv(mytemp[mycount], N+2, MPI_INT, down, 0,
                    mytemp[mycount+1], N+2, MPI_INT, down, 0, comm, &status);
                }
                
            }
            else if(down >= size) {
                MPI_Sendrecv(mytemp[1], N+2, MPI_INT, up, 0,
                    mytemp[0], N+2, MPI_INT, up, 0, comm, &status);
            }
            else {
                MPI_Sendrecv(mytemp[1], N+2, MPI_INT, up, 0,
                    mytemp[0], N+2, MPI_INT, up, 0, comm, &status);
                    
                MPI_Sendrecv(mytemp[mycount], N+2, MPI_INT, down, 0,
                    mytemp[mycount+1], N+2, MPI_INT, down, 0, comm, &status);
            }



            MPI_Barrier(comm);
            ptr = mylife;
            mylife = mytemp;
            mytemp = ptr;
            max_iter = k + 1;
        }
        else {
            max_iter = k + 1;
            k = NTIMES + 1;
            
        }
        MPI_Barrier(comm);
    }
    MPI_Barrier(comm);
    //printwhole(mylife,mycount,N+2,rank);
    MPI_Gatherv(mylife[1], mycount*(N+2), MPI_INT, bufptr, counts, displs, MPI_INT, 0, comm);
    
    t2 = gettime();

    if(rank == 0) {
        //printarray(life,N,N, max_iter);
        printf("Time taken %f seconds for %d iterations\n", t2 - t1, max_iter);
        printf("Writing output to file: %s\n", filename);
        writefile(life, N, fptr);
        fclose(fptr);
        freearray(life);
        freearray(temp);
        printf("Program terminates normally\n") ;
    }
    free(mylife);
    free(mytemp);

    

    MPI_Finalize();
    return 0;
}