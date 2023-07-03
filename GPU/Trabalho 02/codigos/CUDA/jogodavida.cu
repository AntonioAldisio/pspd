/*
    PSPD 2023 - 1
    Alunos: Antonio Aldisio        202028211
            Fernando Miranda Calil 190106565
            Lorrany Oliveira Souza 180113992

    Como compilar:
    $ nvcc jogodavida.cu -o cuda

    Como rodar:
    $ ./cuda

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define ind2d(i,j) (i)*(tam+2)+j
#define POWMIN 3
#define POWMAX 10

double wall_time(void) {
  struct timeval tv;
  struct timezone tz;

  gettimeofday(&tv, &tz);
  return(tv.tv_sec + tv.tv_usec/1000000.0);
}

__global__ void UmaVida(int* tabulIn, int* tabulOut, int tam) {
  int i = blockIdx.x * blockDim.x + threadIdx.x + 1;
  int j = blockIdx.y * blockDim.y + threadIdx.y + 1;
  int vizviv;

  if (i <= tam && j <= tam) {
    vizviv = 	tabulIn[ind2d(i-1,j-1)] + tabulIn[ind2d(i-1,j  )] +
      tabulIn[ind2d(i-1,j+1)] + tabulIn[ind2d(i  ,j-1)] +
      tabulIn[ind2d(i  ,j+1)] + tabulIn[ind2d(i+1,j-1)] +
      tabulIn[ind2d(i+1,j  )] + tabulIn[ind2d(i+1,j+1)];
    if (tabulIn[ind2d(i,j)] && vizviv < 2)
      tabulOut[ind2d(i,j)] = 0;
    else if (tabulIn[ind2d(i,j)] && vizviv > 3)
      tabulOut[ind2d(i,j)] = 0;
    else if (!tabulIn[ind2d(i,j)] && vizviv == 3)
      tabulOut[ind2d(i,j)] = 1;
    else
      tabulOut[ind2d(i,j)] = tabulIn[ind2d(i,j)];
  }
}

void DumpTabul(int * tabul, int tam, int first, int last, char* msg){
  int i, ij;

  printf("%s; Dump posicoes [%d:%d, %d:%d] de tabuleiro %d x %d\n", \
         msg, first, last, first, last, tam, tam);
  for (i=first; i<=last; i++) printf("="); printf("=\n");
  for (i=ind2d(first,0); i<=ind2d(last,0); i+=ind2d(1,0)) {
    for (ij=i+first; ij<=i+last; ij++)
      printf("%c", tabul[ij]? 'X' : '.');
    printf("\n");
  }
  for (i=first; i<=last; i++) printf("="); printf("=\n");
}

void InitTabul(int* tabulIn, int* tabulOut, int tam){
  int ij;

  for (ij=0; ij<(tam+2)*(tam+2); ij++) {
    tabulIn[ij] = 0;
    tabulOut[ij] = 0;
  }

  tabulIn[ind2d(1,2)] = 1; tabulIn[ind2d(2,3)] = 1;
  tabulIn[ind2d(3,1)] = 1; tabulIn[ind2d(3,2)] = 1;
  tabulIn[ind2d(3,3)] = 1;
}

int Correto(int* tabul, int tam){
  int ij, cnt;

  cnt = 0;
  for (ij=0; ij<(tam+2)*(tam+2); ij++)
    cnt = cnt + tabul[ij];
  return (cnt == 5 && tabul[ind2d(tam-2,tam-1)] &&
          tabul[ind2d(tam-1,tam  )] && tabul[ind2d(tam  ,tam-2)] &&
          tabul[ind2d(tam  ,tam-1)] && tabul[ind2d(tam  ,tam  )]);
}

int main(void) {
  int pow;
  int i, tam, *tabulIn, *tabulOut, *d_tabulIn, *d_tabulOut;
  char msg[9];
  double t0, t1, t2, t3;

  for (pow=POWMIN; pow<=POWMAX; pow++) {
    tam = 1 << pow;
    dim3 threadsPerBlock(16, 16);
    dim3 numBlocks((tam + threadsPerBlock.x - 1) / threadsPerBlock.x,
                  (tam + threadsPerBlock.y - 1) / threadsPerBlock.y);

    t0 = wall_time();
    tabulIn  = (int *) malloc ((tam+2)*(tam+2)*sizeof(int));
    tabulOut = (int *) malloc ((tam+2)*(tam+2)*sizeof(int));
    InitTabul(tabulIn, tabulOut, tam);
    t1 = wall_time();

    cudaMalloc((void**)&d_tabulIn, (tam+2)*(tam+2)*sizeof(int));
    cudaMalloc((void**)&d_tabulOut, (tam+2)*(tam+2)*sizeof(int));

    cudaMemcpy(d_tabulIn, tabulIn, (tam+2)*(tam+2)*sizeof(int), cudaMemcpyHostToDevice);

    for (i=0; i<2*(tam-3); i++) {
      UmaVida<<<numBlocks, threadsPerBlock>>>(d_tabulIn, d_tabulOut, tam);
      cudaDeviceSynchronize();
      UmaVida<<<numBlocks, threadsPerBlock>>>(d_tabulOut, d_tabulIn, tam);
      cudaDeviceSynchronize();
    }

    cudaMemcpy(tabulIn, d_tabulIn, (tam+2)*(tam+2)*sizeof(int), cudaMemcpyDeviceToHost);

    if (Correto(tabulIn, tam))
      printf("**RESULTADO CORRETO**\n");
    else
      printf("**RESULTADO ERRADO**\n");

    t2 = wall_time();
    t3 = wall_time();
    printf("tam=%d; tempos: init=%7.7f, comp=%7.7f, fim=%7.7f, tot=%7.7f \n",
           tam, t1-t0, t2-t1, t3-t2, t3-t0);

    cudaFree(d_tabulIn);
    cudaFree(d_tabulOut);
    free(tabulIn);
    free(tabulOut);
  }
  return 0;
}
