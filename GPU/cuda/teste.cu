#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define N 100000
#define MAX_ERR 


__global__ void vector_add(float *out, float *a, float *b, int n) {
    for(int i = 0; i < n; i++){
        out[i] = a[i] + b[i];
    }
}


int main(){
    float *a, *b, *out; 

    // Allocate memory
    a   = (float*)malloc(sizeof(float) * N);
    b   = (float*)malloc(sizeof(float) * N);
    out = (float*)malloc(sizeof(float) * N);

    // Initialize array
    for(int i = 0; i < N; i++){
        a[i] = 1.0f;
        b[i] = 2.0f;
    }

    // Main function
    vector_add<<<1,1024>>>(out, a, b, N);

    // Verification
  cudaDeviceSynchronize();
    printf("out[0] = %f\n", out[N]);
    printf("PASSED\n");
}
