#include <stdio.h>
#include <stdlib.h>
#include <omp.h>



int calcular_offset() {
    return rand() % 15 + 1;
}



int main(int argc, char** argv) {
    int matrix[100][8];
    #pragma omp parrallel
    {
        int tid = omp_get_thread_num();
        if (tid == 0){
            for(int i =0;i <100; i++){
                for(int j =0; j<8; j++){
                    matrix[i][j] = i+j;
                }
            }        
        }
    }
    #pragma omp for
    for(int i =0;i <100; i++){
        for(int j =0; j<8; j++){
            printf("%d \n", matrix[i][j]);
        }
    }

}