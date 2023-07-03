/*
    PSPD 2023 - 1
    Alunos: Antonio Aldisio        202028211
            Fernando Miranda Calil 190106565
            Lorrany Oliveira Souza 180113992

    Como compilar:
    $ gcc jogodavidaocl.c -o vida -lm -lOpenCL -DCL_TARGET_OPENCL_VERSION=300

    Como rodar:
    $ ./vida
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <CL/cl.h>

#define ind2d(i,j) (i)*(tam+2)+j
#define POWMIN 3
#define POWMAX 10

double wall_time(void) {
  struct timeval tv;
  struct timezone tz;

  gettimeofday(&tv, &tz);
  return(tv.tv_sec + tv.tv_usec/1000000.0);
}

void DumpTabul(int * tabul, int tam, int first, int last, char* msg){
  int i, ij;

  printf("%s; Dump posicoes [%d:%d, %d:%d] de tabuleiro %d x %d\n",
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
  int i, tam, *tabulIn, *tabulOut;
  char msg[9];
  double t0, t1, t2, t3;

  cl_int err;

  cl_platform_id platform_id;
  cl_device_id device_id;
  cl_context context;
  cl_command_queue command_queue;
  cl_program program;
  cl_kernel kernel;
  cl_mem bufTabulIn, bufTabulOut;

  // para todos os tamanhos do tabuleiro

  for (pow=POWMIN; pow<=POWMAX; pow++) {
    tam = 1 << pow;
    // aloca e inicializa tabuleiros
    t0 = wall_time();
    tabulIn  = (int *) malloc ((tam+2)*(tam+2)*sizeof(int));
    tabulOut = (int *) malloc ((tam+2)*(tam+2)*sizeof(int));
    InitTabul(tabulIn, tabulOut, tam);
    t1 = wall_time();

    // Configuração do OpenCL

    // Obtenha a plataforma e o dispositivo da Nvidia
    err = clGetPlatformIDs(1, &platform_id, NULL);
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

    // Crie um contexto OpenCL
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);

    // Crie uma fila de comando OpenCL
    command_queue = clCreateCommandQueue(context, device_id, 0, &err);

    // Carregue o arquivo de kernel .cl
    FILE *file = fopen("kernel.cl", "r");
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);
    char *source = (char *)malloc(file_size + 1);
    fread(source, sizeof(char), file_size, file);
    source[file_size] = '\0';

    // Crie um programa OpenCL
    program = clCreateProgramWithSource(context, 1, (const char **)&source, NULL, &err);

    // Compile o programa OpenCL
    err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // Crie o kernel OpenCL
    kernel = clCreateKernel(program, "UmaVida", &err);

    // Criar buffers de memória para tabulIn e tabulOut na memória do dispositivo
    bufTabulIn = clCreateBuffer(context, CL_MEM_READ_WRITE,
                                (tam+2)*(tam+2)*sizeof(int), NULL, &err);
    bufTabulOut = clCreateBuffer(context, CL_MEM_READ_WRITE,
                                 (tam+2)*(tam+2)*sizeof(int), NULL, &err);


    // Transferir os dados de entrada para os buffers de memória do dispositivo
    err = clEnqueueWriteBuffer(command_queue, bufTabulIn, CL_TRUE, 0,
                               (tam+2)*(tam+2)*sizeof(int), tabulIn, 0, NULL, NULL);

    // Definir os argumentos do kernel
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&bufTabulIn);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bufTabulOut);
    err = clSetKernelArg(kernel, 2, sizeof(int), (void *)&tam);

    // Executar o kernel OpenCL
    size_t global_item_size = (tam * tam);
    err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
                                &global_item_size, NULL, 0, NULL, NULL);


    // Ler os resultados de volta para a memória do host
    err = clEnqueueReadBuffer(command_queue, bufTabulIn, CL_TRUE, 0,
                              (tam+2)*(tam+2)*sizeof(int), tabulOut, 0, NULL, NULL);


    t2 = wall_time();

    if (!Correto(tabulOut, tam))
      printf("**RESULTADO CORRETO**\n");
    else
      printf("**RESULTADO ERRADO**\n");

    t3 = wall_time();
    printf("tam=%d; tempos: init=%7.7f, comp=%7.7f, fim=%7.7f, tot=%7.7f \n",
           tam, t1-t0, t2-t1, t3-t2, t3-t0);

    // Liberar recursos do OpenCL
    clReleaseMemObject(bufTabulIn);
    clReleaseMemObject(bufTabulOut);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);

    free(tabulIn);
    free(tabulOut);
  }
  return 0;
}
