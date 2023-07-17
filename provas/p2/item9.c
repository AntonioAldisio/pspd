#include<stdio.h>
#include <omp.h>


int main(void){
    printf("%d\n", omp_get_num_threads());
    #pragma omp parrallel
    {

    }
    printf("%d\n", omp_get_num_threads());
    return 0;
}