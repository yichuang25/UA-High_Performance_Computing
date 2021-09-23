#include <stdio.h>
#include <stdlib.h>
#include "/usr/local/opt/libomp/include/omp.h"

int main(int argc, char *argv[]) {
    //global variable
    int thread_count = atoi(argv[1]);
    int sum = 0;

    #pragma omp parallel num_threads(thread_count) 
    {
        //thread local variable
        //every thread has a copy
        int tid = omp_get_thread_num();
        int numthreads = omp_get_num_threads();

        //critical section
        #pragma omp critical
        sum+=tid;
        #pragma omp single
        printf("Hello, World! From %d of %d\n",tid, numthreads);
    }
    printf("The sum is %d\n", sum);
    return 0;
}