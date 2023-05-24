#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#define OUTFILE "out_julia_normal.bmp"

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
} /*fim compute julia pixel */

int write_bmp_header(FILE *f, int largura, int altura) {

  unsigned int row_size_in_bytes = largura * 3 +
	  ((largura * 3) % 4 == 0 ? 0 : (4 - (largura * 3) % 4));

  // Define all fields in the bmp header
  char id[2] = "BM";
  unsigned int filesize = 54 + (int)(row_size_in_bytes * altura * sizeof(char));
  short reserved[2] = {0,0};
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
    int n, rank, size;
    int area = 0, largura = 0, altura = 0, local_i = 0;
    FILE *output_file;
    unsigned char *pixel_array, *rgb;

    MPI_Init(&argc, &argv); // Inicialização do MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtenção do rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtenção do número total de processos

    if ((argc <= 1) || (atoi(argv[1]) < 1)) {
        if (rank == 0) {
            fprintf(stderr,"Entre 'N' como um inteiro positivo! \n");
        }
        MPI_Finalize(); // Finalização do MPI
        return -1;
    }

    n = atoi(argv[1]);
    altura = n;
    largura = 2 * n;
    area = altura * largura * 3;

    // Divisão do trabalho entre os processos
    int local_height = altura / size;
    int local_start = rank * local_height;
    int local_end = local_start + local_height;

    if (rank == size - 1) {
        local_end = altura;
    }

    // Alocação de memória para o array de pixels e o array RGB
    pixel_array = calloc(local_height * largura * 3, sizeof(unsigned char));
    rgb = calloc(3, sizeof(unsigned char));

    // Processamento dos pixels
    for (int i = local_start; i < local_end; i++) {
        for (int j = 0; j < largura * 3; j += 3) {
            compute_julia_pixel(j / 3, i, largura, altura, 1.0, rgb);
            pixel_array[local_i] = rgb[0];
            local_i++;
            pixel_array[local_i] = rgb[1];
            local_i++;
            pixel_array[local_i] = rgb[2];
            local_i++;
        }
    }

    // Liberação de memória do array RGB
    free(rgb);

    // Gravação serial do arquivo por ordem de rank
    if (rank == 0) {
        output_file = fopen(OUTFILE, "w");
        write_bmp_header(output_file, largura, altura);
    }

    // Espera para garantir que o arquivo seja aberto antes da gravação
    MPI_Barrier(MPI_COMM_WORLD);

    // Gravação dos dados do array de pixels no arquivo
    MPI_File output_mpi_file;
    MPI_File_open(MPI_COMM_WORLD, OUTFILE, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &output_mpi_file);
    MPI_File_seek(output_mpi_file, local_start * largura * 3, MPI_SEEK_SET);
    MPI_File_write(output_mpi_file, pixel_array, (local_end - local_start) * largura * 3, MPI_UNSIGNED_CHAR, MPI_STATUS_IGNORE);
    MPI_File_close(&output_mpi_file);

    // Liberação de memória do array de pixels
    free(pixel_array);

    // Finalização do MPI
    MPI_Finalize();

    return 0;
}
