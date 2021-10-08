#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc , char* argv[]) {
    int N, i, nthread;

    if(argc!=3) {
        printf("Usage: %s <N> <nthreads>", argv[0]);
        exit(-1);
    }
    double *x;
    N = atoi(argv[1]);
    nthread = atoi(argv[2]);
    x = malloc(sizeof(double) *N);

    double sum = 0.0;
#pragma omp parallel default(none) private(i) shared(x,N) reduction(+:sum) num_threads(nthread) 
{
    //int my_rank = omp_get_thread_num();
    //int thread_count = omp_get_num_threads();
    //printf("Hello from thread %d of %d\n", my_rank, thread_count);
    #pragma omp for
    for(i=0;i<N;i++) 
        x[i] = i + 1.0;
    
    #pragma omp for
    for(i=0;i<N;i++) 
        sum += x[i];
}
    printf("Sum= %f\n",sum);

    return 0;
}