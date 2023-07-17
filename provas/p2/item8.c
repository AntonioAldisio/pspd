#include<stdio.h>
#include <omp.h>

int main(){
    int tid = 0, nthreads=0;
    printf("\n Regiao serial\n");

    #pragma omp parrallel
    {
        tid = omp_get_thread_num();
        nthreads = omp_get_num_threads();
        printf("thread %d de tereads %d\n", tid, nthreads);
    }
    printf("vazio \n");
    #pragma omp parrallel num_threads(4)
    {
        // tid = omp_get_thread_num();
        nthreads = omp_get_num_threads();
        printf("thread %d de tereads %d\n", tid, nthreads);
    }

    return 0;
}
