#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#define OUTFILE "out_julia_normal.bmp"

int compute_julia_pixel(int x, int y, int largura, int altura, float tint_bias, unsigned char *rgb) {
  // Implementação do código para o cálculo de um pixel da imagem Julia

  // Resto do código permanece inalterado
}

int write_bmp_header(FILE *f, int largura, int altura) {
  // Implementação do código para escrever o cabeçalho BMP no arquivo

  // Resto do código permanece inalterado
}

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
