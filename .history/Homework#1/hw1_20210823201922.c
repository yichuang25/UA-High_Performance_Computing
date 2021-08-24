/*
* Name: Yichen Huang
* Email: yhuang87@crimson.ua.edu
* Course Section: CS 481
* Homework#: 1
* Instruction to Compile:
* Instruction to Execute:
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>

static void printArray(int **array, int size) {
    for(int i=0;i<size;i++) {
        for(int j=0;j<size;j++) {
            printf("%d ", array[i][j]);
        }
        printf("\n");
    }
}

static void printGraph(int **cells, int size) {
    for(int i=1;i<size-1;i++) {
        for(int j=1;j<size-1;j++) {
            if(cells[i][j] == 0) {
                printf(" ");
            }
            else {
                printf("*");
            }
        }
        printf("\n");
    }
}

bool compareGraph(int **cell, int **temp, int size) { //compare graph is same->true / different->false
    for(int i=0;i<size;i++) {
        for(int j=0;j<size;j++) {
            if(cell[i][j] != temp[i][j]) {
                return false;
            }
        }
    }
    return true;
}

int **allocateArray(int C, int R) {     //Remind reference from hw1.c from professor
    int i;
    int *p, **a;

    p = (int *)malloc(C*R*sizeof(int));
    a = (int **)malloc(C*sizeof(int*));

    if(p==NULL || a==NULL) {
        printf("Memory Allocating Error\n");
    }

    for(i=0;i<C;i++) {
        a[i] = &p[i*R];
    }

    return a;
    
}

void initializeArray(int **array, int size) {
    for(int i=0;i<size;i++) {
        for(int j=0;j<size;j++) {
            array[i][j] = 0;
        }
    }
}

void freeArray(int **array) {       //Remind reference from hw1.c from professor
    free(&array[0][0]);
    free(array);
}

bool updateCell(int **cell,int size) { //if same -> true
    int **temp = NULL;
    temp = allocateArray(size,size);
    initializeArray(temp,size);

    int Neighbour = 0;
    for(int i=1;i<size-1;i++) {
        for(int j=1;j<size-1;j++) {
            Neighbour = cell[i-1][j-1] + cell[i-1][j] + cell[i-1][j+1]+
                        cell[i][j-1] + cell[i][j+1] +
                        cell[i+1][j-1] + cell[i+1][j] + cell[i+1][j+1];
            
            if(Neighbour == 3) {
                temp[i][j] = 1;
            }
            else if (Neighbour == 2) {
                temp[i][j] = cell[i][j];
            }
            else {
                temp[i][j] = 0;
            }
        }
    }

    if(compareGraph(cell,temp,size) == true) {
        freeArray(temp);
        return true;
    }

    for(int i=1;i<size-1;i++) {
        for(int j=1;j<size-1;j++) {
            cell[i][j] = temp[i][j];
        }
    }
    
    freeArray(temp);
    return false;
}

double gettime() {
  struct timeval tval;

  gettimeofday(&tval, NULL);

  return( (double)tval.tv_sec + (double)tval.tv_usec/1000000.0 );
}


int main(int argc, char **argv) {

    //check command line input
    if(argc != 3) {
        printf("Usage: %s <SIZE> <MAX_GEN>\n",argv[0]);
        exit(-1);
    }

    double start,end;

    //get size of problem and max generations
    int size = atoi(argv[1]) + 2;
    int max_gen = atoi(argv[2]);

    int **cells = NULL;
    cells = allocateArray(size,size);

    //initialze array
    initializeArray(cells,size);

    
    
    //Randomly assign live cell
    srand(time(NULL));
    for(int i=1;i<size-1;i++) {
        for(int j=1;j<size-1;j++) {
            cells[i][j] = rand() % 2;
        }
    }
    start = gettime();

    for(int i=0; i<max_gen;i++) {
        bool result;
        printf("In Gen# %d\n",i);
        printGraph(cells,size);
        result = updateCell(cells,size);
        if(result == true) {
            printf("In Gen# %d\n",i+1);
            printGraph(cells,size);
            break;
        }
    }

    end = gettime();
        
    freeArray(cells);
    printf("Time taken for size %d = %lf seconds\n", size-2, end-start);

    return 0;
}