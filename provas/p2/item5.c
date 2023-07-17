#include<stdio.h>
#include <omp.h>
#include <string.h>
#define TAM 12

int main(){
    int A[TAM], B [TAM], C[TAM];
    int i;

    for (i=0; i < TAM; i++){
        A[i]=2*i - 1;
        B[i]=i + 2;
    }
    #pragma omp parrallel private(i) reduction(+:C)

    {
        int tid = omp_get_thread_num();
        // #pragma omp for
        for (i =0; i< TAM; i++){
            C[i] = A[i] + B[i];
            printf("Thread[%d] calculou C[%d] \n", tid, i);
        }
    }
    for (i =0; i < TAM; i++){
        printf("C [%d]=%d \n", i, C[i]);
    }


}