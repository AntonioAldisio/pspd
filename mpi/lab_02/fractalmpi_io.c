/*
    PSPD 2023 - 1
    Alunos: Antonio Aldisio        202028211
            Fernando Miranda Calil 190106565
            Lorrany Oliveira Souza 180113992

    Como compilar:
    $ mpicc fractalmpi_io.c -o fractalmpi_io -lm

    Como rodar:
    $ mpirun -np 4 fractalmpi_io 1000
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define OUTFILE "out_julia_parallel.bmp"

int compute_julia_pixel(int x, int y, int largura, int altura, float tint_bias, unsigned char *rgb) {
  // Check coordinates
  if ((x < 0) || (x >= largura) || (y < 0) || (y >= altura)) {
    fprintf(stderr,"Invalid (%d,%d) pixel coordinates in a %d x %d image\n", x, y, largura, altura);
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


int main(int argc, char *argv[]) {
    int rank, size, n;
    int area = 0, largura = 0, altura = 0, local_i = 0;
    unsigned char *pixel_array, *rgb;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if ((argc <= 1) || (atoi(argv[1]) < 1)) {
        fprintf(stderr, "Entre 'N' como um inteiro positivo! \n");
        MPI_Finalize();
        return -1;
    }

    n = atoi(argv[1]);
    altura = n;
    largura = 2 * n;
    area = altura * largura * 3;

    // Calcula o número de linhas por processo
    int rows_per_process = altura / size;
    int remaining_rows = altura % size;

    // Calcula o deslocamento local para cada rank
    int local_offset = rank * rows_per_process;

    // Calcula o número de linhas para cada rank
    int num_rows = rows_per_process + (rank < remaining_rows ? 1 : 0);

    // Aloca memória para o array local de pixels
    int local_area = num_rows * largura * 3;
    pixel_array = calloc(local_area, sizeof(unsigned char));
    rgb = calloc(3, sizeof(unsigned char));

    // Computa os pixels para o rank atual
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < largura * 3; j += 3) {
            compute_julia_pixel(j / 3, local_offset + i, largura, altura, 1.0, rgb);
            pixel_array[local_i] = rgb[0];
            local_i++;
            pixel_array[local_i] = rgb[1];
            local_i++;
            pixel_array[local_i] = rgb[2];
            local_i++;
        }
    }

    // Cria um arquivo MPI
    MPI_File output_file;
    MPI_File_open(MPI_COMM_WORLD, OUTFILE, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &output_file);

    // Calcula o deslocamento global para cada rank
    MPI_Offset global_offset;
    MPI_Exscan(&local_area, &global_offset, 1, MPI_OFFSET, MPI_SUM, MPI_COMM_WORLD);
    global_offset += local_offset * largura * 3;

    // Grava os pixels em paralelo, respeitando o deslocamento global
    MPI_File_set_view(output_file, global_offset, MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, "native");
    MPI_File_write_all(output_file, pixel_array, local_area, MPI_UNSIGNED_CHAR, MPI_STATUS_IGNORE);

    // Fecha o arquivo MPI
    MPI_File_close(&output_file);

    // Libera a memória alocada
    free(rgb);
    free(pixel_array);

    MPI_Finalize();
    return 0;
}
