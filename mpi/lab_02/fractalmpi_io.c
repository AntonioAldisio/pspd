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

    // Calcula o deslocamento (offset) para cada rank
    int offset = rank * rows_per_process;

    // Calcula o número de linhas para cada rank
    int num_rows = rows_per_process + (rank < remaining_rows ? 1 : 0);

    // Aloca memória para o array local de pixels
    int local_area = num_rows * largura * 3;
    pixel_array = calloc(local_area, sizeof(unsigned char));
    rgb = calloc(3, sizeof(unsigned char));

    // Computa os pixels para o rank atual
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < largura * 3; j += 3) {
            compute_julia_pixel(j / 3, offset + i, largura, altura, 1.0, rgb);
            pixel_array[local_i] = rgb[0];
            local_i++;
            pixel_array[local_i] = rgb[1];
            local_i++;
            pixel_array[local_i] = rgb[2];
            local_i++;
        }
    }

    // Cria um tipo de dado MPI para representar um pixel (3 unsigned chars)
    MPI_Datatype pixel_type;
    MPI_Type_contiguous(3, MPI_UNSIGNED_CHAR, &pixel_type);
    MPI_Type_commit(&pixel_type);

    // Cria um arquivo MPI
    MPI_File output_file;
    MPI_File_open(MPI_COMM_WORLD, OUTFILE, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &output_file);

    // Calcula o deslocamento em bytes para o rank atual
    MPI_Offset displacement = offset * largura * 3 * sizeof(unsigned char);

    // Grava os pixels no arquivo usando MPI_File_write_ordered
    MPI_File_set_view(output_file, displacement, MPI_UNSIGNED_CHAR, pixel_type, "native");
    MPI_File_write_ordered(output_file, pixel_array, local_area / 3, pixel_type, MPI_STATUS_IGNORE);

    // Fecha o arquivo
    MPI_File_close(&output_file);

    // Libera a memória alocada
    free(rgb);
    free(pixel_array);

    MPI_Type_free(&pixel_type);
    MPI_Finalize();
    return 0;
}
