#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MATRIX_SIZE 16
#define ROOT 0

void initializeMatrix(float *matrix) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix[i * MATRIX_SIZE + j] = (float)rand() / RAND_MAX;  // Inicializa com valores randômicos entre 0 e 1
        }
    }
}

void printMatrix(float *matrix) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%.2f ", matrix[i * MATRIX_SIZE + j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int numProcesses, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (numProcesses != 5) {
        printf("Este programa deve ser executado com 5 processos: 1 master e 4 workers.\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == ROOT) {
        // Processo master
        float *matrixA = (float *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(float));
        float *matrixB = (float *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(float));
        float *matrixC = (float *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(float));

        initializeMatrix(matrixA);
        initializeMatrix(matrixB);

        // Distribuição dos dados para os workers
        int chunkSize = MATRIX_SIZE / (numProcesses - 1);
        for (int i = 1; i < numProcesses; i++) {
            MPI_Send(matrixA + (i - 1) * chunkSize * MATRIX_SIZE, chunkSize * MATRIX_SIZE, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
            MPI_Send(matrixB + (i - 1) * chunkSize * MATRIX_SIZE, chunkSize * MATRIX_SIZE, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
        }

        // Recebimento dos resultados dos workers
        for (int i = 1; i < numProcesses; i++) {
            MPI_Recv(matrixC + (i - 1) * chunkSize * MATRIX_SIZE, chunkSize * MATRIX_SIZE, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Impressão da matriz C resultante
        printf("Matriz C:\n");
        printMatrix(matrixC);

        free(matrixA);
        free(matrixB);
        free(matrixC);
    } else {
        // Processo worker
        int chunkSize = MATRIX_SIZE / (numProcesses - 1);
        float *matrixA_chunk = (float *)malloc(chunkSize * MATRIX_SIZE * sizeof(float));
        float *matrixB_chunk = (float *)malloc(chunkSize * MATRIX_SIZE * sizeof(float));
        float *matrixC_chunk = (float *)malloc(chunkSize * MATRIX_SIZE * sizeof(float));

        // Recebimento dos dados do processo master
        MPI_Recv(matrixA_chunk, chunkSize * MATRIX_SIZE, MPI_FLOAT, ROOT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(matrixB_chunk, chunkSize * MATRIX_SIZE, MPI_FLOAT, ROOT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Soma dos elementos das matrizes A e B
        for (int i = 0; i < chunkSize; i++) {
            for (int j = 0; j < MATRIX_SIZE; j++) {
                matrixC_chunk[i * MATRIX_SIZE + j] = matrixA_chunk[i * MATRIX_SIZE + j] + matrixB_chunk[i * MATRIX_SIZE + j];
            }
        }

        // Envio do resultado para o processo master
        MPI_Send(matrixC_chunk, chunkSize * MATRIX_SIZE, MPI_FLOAT, ROOT, 0, MPI_COMM_WORLD);

        free(matrixA_chunk);
        free(matrixB_chunk);
        free(matrixC_chunk);
    }

    MPI_Finalize();
    return 0;
}
