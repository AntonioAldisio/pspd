#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define NUM_STEPS 8000000

int main(int argc, char *argv[]) {
    int numProcesses, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double x, pi, sum = 0.0, globalSum = 0.0;
    double step;

    if (rank == 0) {
        step = 1.0 / (double)NUM_STEPS;
        for(int i = 0; i < NUM_STEPS; i++) {
            x = (i + 0.5) * step;
            sum += 4 / (1.0 + x * x);
        }

        MPI_Reduce(&sum, &globalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    } else {
        step = 1.0 / (double)NUM_STEPS;
        for(int i = rank; i < NUM_STEPS; i += numProcesses) {
            x = (i + 0.5) * step;
            sum += 4 / (1.0 + x * x);
        }

        MPI_Reduce(&sum, NULL, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        pi = sum * step;
        printf("Rank[%d] - Pi = %f\n", rank, pi);
    }

    MPI_Finalize();
    return 0;
}
