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

int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Usage: %s <SIZE> <MAX_GEN>\n",argv[0]);
        exit(-1);
    }

    //get size of problem and max generations
    int size = atoi(argv[1]);
    int max_gen = atoi(argv[2]);

    int cells[size][size];

    srand(time(NULL));

        


    return 0;
}