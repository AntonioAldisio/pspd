/*
    PSPD 2023 - 1
    Alunos: Antonio Aldisio        202028211
            Fernando Miranda Calil 190106565
            Lorrany Oliveira Souza 180113992

    Como compilar:
    $ mpicc -o item2 item2.c

    Como rodar:
    $ mpirun -np 4 ./item2
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define VETOR_SIZE 100

int calcular_offset() {
    return rand() % 15 + 1;
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL));

    if (rank == 0) {
        int vetor[VETOR_SIZE];

        for (int i = 0; i < VETOR_SIZE; i++) {
            vetor[i] = i;
        }

        int offsets[size];
        offsets[0] = 0;
        for (int worker = 1; worker < size; worker++) {
            offsets[worker] = offsets[worker - 1] + calcular_offset();
        }

        for (int worker = 1; worker < size; worker++) {
            MPI_Send(&offsets[worker], 1, MPI_INT, worker, 0, MPI_COMM_WORLD);
        }

        for (int i = 0; i < offsets[1]; i++) {
            printf("Worker %d - Vetor[%d] = %d\n", rank, i, vetor[i]);
        }
    }
    else {
        int offset;
        MPI_Recv(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int end = (rank == size - 1) ? VETOR_SIZE : offset + calcular_offset();
        for (int i = offset; i < end; i++) {
            printf("Worker %d - Vetor[%d] = %d\n", rank, i, i);
        }
    }

    MPI_Finalize();
    return 0;
}
