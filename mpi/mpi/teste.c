#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int dado = 42;
    int destino = 1;
    int tag = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // Processo de rank 0 envia dado para o processo de rank 1
        MPI_Send(&dado, 1, MPI_INT, destino, tag, MPI_COMM_WORLD);
        printf("Processo %d enviou o dado %d para o processo %d\n", rank, dado, destino);
    } else if (rank == destino) {
        // Processo de rank 1 recebe o dado do processo de rank 0
        MPI_Recv(&dado, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Processo %d recebeu o dado %d do processo 0\n", rank, dado);
    }

    MPI_Finalize();

    return 0;
}
