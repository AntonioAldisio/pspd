/*
    PSPD 2023 - 1
    Alunos: Antonio Aldisio        202028211
            Fernando Miranda Calil 190106565
            Lorrany Oliveira Souza 180113992

    Como compilar:
    $ nvcc fractalcuda.cu -o fractalcuda -lm

    Como rodar:
    $ cuda-memcheck ./fractalcuda 1000

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cuda_runtime.h>
#define OUTFILE "out_julia_normal_cu.bmp"

__device__ int compute_julia_pixel(int x, int y, int largura, int altura, float tint_bias, unsigned char *rgb) {
  // Check coordinates
  if ((x < 0) || (x >= largura) || (y < 0) || (y >= altura)) {
    printf("Invalid (%d,%d) pixel coordinates in a %d x %d image\n", x, y, largura, altura);
    return -1;
  }
  // "Zoom in" to a pleasing view of the Julia set
  float X_MIN = -1.6, X_MAX = 1.6, Y_MIN = -0.9, Y_MAX = +0.9;
  float float_y = (Y_MAX - Y_MIN) * (float)y / altura + Y_MIN ;
  float float_x = (X_MAX - X_MIN) * (float)x / largura  + X_MIN ;
  // Point that defines the Julia set
  float julia_real = -.79;
  float julia_img = .15;
  // Maximum number of iteration
  int max_iter = 300;
  // Compute the complex series convergence
  float real=float_y, img=float_x;
  int num_iter = max_iter;
  while (( img * img + real * real < 2 * 2 ) && ( num_iter > 0 )) {
    float xtemp = img * img - real * real + julia_real;
    real = 2 * img * real + julia_img;
    img = xtemp;
    num_iter--;
  }

  // Paint pixel based on how many iterations were used, using some funky colors
  float color_bias = (float) num_iter / max_iter;
  rgb[0] = (num_iter == 0 ? 200 : - 500.0 * pow(tint_bias, 1.2) *  pow(color_bias, 1.6));
  rgb[1] = (num_iter == 0 ? 100 : -255.0 *  pow(color_bias, 0.3));
  rgb[2] = (num_iter == 0 ? 100 : 255 - 255.0 * pow(tint_bias, 1.2) * pow(color_bias, 3.0));

  return 0;
}

__global__ void compute_julia_pixels(unsigned char *pixel_array, int largura, int altura) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;
  int area = largura * altura;
  if (x < largura && y < altura) {
    int local_i = (y * largura + x) * 3;
    unsigned char rgb[3];
    compute_julia_pixel(x, y, largura, altura, 1.0, rgb);
    pixel_array[local_i] = rgb[0];
    pixel_array[local_i + 1] = rgb[1];
    pixel_array[local_i + 2] = rgb[2];
  }
}

int write_bmp_header(FILE *f, int largura, int altura) {
  unsigned int row_size_in_bytes = largura * 3 + ((largura * 3) % 4 == 0 ? 0 : (4 - (largura * 3) % 4));

  // Define all fields in the bmp header
    char id[3] = "BM"; // Alterado o tamanho para 3
    unsigned int filesize = 54 + (int)(row_size_in_bytes * altura * sizeof(char));
    short reserved[2] = {0, 0};
    unsigned int offset = 54;

    unsigned int size = 40;
    unsigned short planes = 1;
    unsigned short bits = 24;
    unsigned int compression = 0;
    unsigned int image_size = largura * altura * 3 * sizeof(char);
    int x_res = 0;
    int y_res = 0;
    unsigned int ncolors = 0;
    unsigned int importantcolors = 0;


  // Write the bytes to the file, keeping track of the
  // number of written "objects"
  size_t ret = 0;
  ret += fwrite(id, sizeof(char), 2, f);
  ret += fwrite(&filesize, sizeof(int), 1, f);
  ret += fwrite(reserved, sizeof(short), 2, f);
  ret += fwrite(&offset, sizeof(int), 1, f);
  ret += fwrite(&size, sizeof(int), 1, f);
  ret += fwrite(&largura, sizeof(int), 1, f);
  ret += fwrite(&altura, sizeof(int), 1, f);
  ret += fwrite(&planes, sizeof(short), 1, f);
  ret += fwrite(&bits, sizeof(short), 1, f);
  ret += fwrite(&compression, sizeof(int), 1, f);
  ret += fwrite(&image_size, sizeof(int), 1, f);
  ret += fwrite(&x_res, sizeof(int), 1, f);
  ret += fwrite(&y_res, sizeof(int), 1, f);
  ret += fwrite(&ncolors, sizeof(int), 1, f);
  ret += fwrite(&importantcolors, sizeof(int), 1, f);

  // Success means that we wrote 17 "objects" successfully
  return (ret != 17);
} /* fim write bmp-header */

int main(int argc, char *argv[]) {
  int n;
  int area = 0, largura = 0, altura = 0;
  FILE *output_file;
  unsigned char *pixel_array, *d_pixel_array;

  if ((argc <= 1) || (atoi(argv[1]) < 1)) {
    fprintf(stderr, "Entre 'N' como um inteiro positivo! \n");
    return -1;
  }
  n = atoi(argv[1]);
  altura = n; largura = 2 * n; area = altura * largura * 3;

  // Allocate host memory for the pixel array
  pixel_array = (unsigned char*)malloc(area * sizeof(unsigned char));

  // Allocate device memory for the pixel array
  cudaMalloc((void**)&d_pixel_array, area * sizeof(unsigned char));

  dim3 threadsPerBlock(2, 2);
  dim3 numBlocks((largura + threadsPerBlock.x - 1) / threadsPerBlock.x, (altura + threadsPerBlock.y - 1) / threadsPerBlock.y);

  printf("Computando linhas de pixel %d até %d, para uma área total de %d\n", 0, n-1, area);

  // Copy the pixel array from host to device
  cudaMemcpy(d_pixel_array, pixel_array, area * sizeof(unsigned char), cudaMemcpyHostToDevice);

  // Start measuring the execution time
  clock_t start = clock();

  // Launch the kernel
  compute_julia_pixels<<<numBlocks, threadsPerBlock>>>(d_pixel_array, largura, altura);

  // Synchronize to make sure the kernel finishes execution
  cudaDeviceSynchronize();

  // Stop measuring the execution time
  clock_t end = clock();

  // Calculate the elapsed time in seconds
  double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
  printf("Tempo de execucao: %.4f segundos\n", elapsed_time);

  // Copy the pixel array from device to host
  cudaMemcpy(pixel_array, d_pixel_array, area * sizeof(unsigned char), cudaMemcpyDeviceToHost);

  // Free device memory
  cudaFree(d_pixel_array);

  // Write the BMP file
  output_file = fopen(OUTFILE, "w");
  write_bmp_header(output_file, largura, altura);
  fwrite(pixel_array, sizeof(unsigned char), area, output_file);
  fclose(output_file);

  // Free host memory
  free(pixel_array);

  return 0;
}