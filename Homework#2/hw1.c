/*
* Name: Yichen Huang
* Email: yhuang87@crimson.ua.edu
* Course Section: CS 481
* Homework#: 1
* Instruction to Compile: gcc -Wall -std=c99 -o hw1 hw1.c
* Instruction to Execute: ./hw1 <size> <max_gen>
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>

static void printArray(int **array, int size) { //print the array with 0(dead) and 1 (alive)
    for(int i=1;i<size-1;i++) {
        for(int j=1;j<size-1;j++) {
            printf("%d ", array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int **allocateArray(int C, int R) {     //dynamically allocate the memory (modify from hw1.c from blakboard)
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

void initializeArray(int **array, int size) { // initialize whole array with all 0
    for(int i=0;i<size;i++) {
        for(int j=0;j<size;j++) {
            array[i][j] = 0;
        }
    }
}

void freeArray(int **array) {       //release the dynamically allocated memory (copy from hw1.c from blakboard)
    free(&array[0][0]);
    free(array);
}

double gettime() { // get the current time (copy from hw1.c from blakboard)
  struct timeval tval;

  gettimeofday(&tval, NULL);

  return( (double)tval.tv_sec + (double)tval.tv_usec/1000000.0 );
}

void randomlize(int **array, int size) { // randomlize each cell in the graph
    srand(2);
    for(int i=1;i<size-1;i++) {
        for(int j=1;j<size-1;j++) {
            array[i][j] = rand() % 2;
        }
    }
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

    int **life1 = NULL;
    int **life2 = NULL;
    life1 = allocateArray(size,size);
    life2 = allocateArray(size,size);


    //initialze array
    initializeArray(life1,size);
    initializeArray(life2,size);
    
    randomlize(life1,size);
    //printGraph(cells,size);
    
    start = gettime();
    //printf("In Gen# 0\n");
    //printArray(life1,size);

    for(int i=1; i<=max_gen;i++) {
        int neighbor = 0;
        bool result = false;
        for(int i=1;i<size-1;i++) {
            for(int j=1;j<size-1;j++) {
                neighbor = life1[i-1][j-1] + life1[i-1][j] + life1[i-1][j+1]+
                           life1[i][j-1] + life1[i][j+1] +
                           life1[i+1][j-1] + life1[i+1][j] + life1[i+1][j+1];
                
                if(neighbor == 3) {
                    life2[i][j] = 1;
                }
                else if(neighbor == 2) {
                    life2[i][j] = life1[i][j];
                }
                else {
                    life2[i][j] = 0;
                }

                if(life1[i][j] != life2[i][j]) { //check if graph changed
                    result = true;
                }
            }
        }

        int** temp = life2;
        life2 = life1;
        life1 = temp;
        

        if(result == false) {
            break;
        }
        //printf("In Gen# %d\n",i);
        //printArray(life1,size);
    }

    end = gettime();
    //printGraph(cells,size);
    freeArray(life1);
    freeArray(life2);
    printf("Time taken for size %d is %lf seconds\n", size-2, end-start);

    return 0;
}