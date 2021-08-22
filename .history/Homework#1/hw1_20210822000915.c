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

static void printArray(int **array, int size) {
    for(int i=0;i<size;i++) {
        for(int j=0;j<size;j++) {
            printf("%d ", array[i][j])
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

void updateCell(int **cell,int size) {
    int **temp = NULL;
    temp = allocateArray(size,size);
    initializeArray(temp,size);

    int Neighbour = 0;
    for(int i=1;i<size-1;i++) {
        for(int j=1;j<size-1;j++) {
            Neighbour = cell[i-1][j-1] + cell[i-1][j] + cell[i-1][j+1]+
                        cell[i][j-1] + cell[i][j+1] +
                        cell[i+1][j-1] + cell[i+1][j] + cell[i+1][j+1];
        }
    }

    
}


int main(int argc, char **argv) {
    if(argc != 3) {
        printf("Usage: %s <SIZE> <MAX_GEN>\n",argv[0]);
        exit(-1);
    }

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

    printGraph(cells,size);
    printArray(cells,size);
        
    freeArray(cells);

    return 0;
}