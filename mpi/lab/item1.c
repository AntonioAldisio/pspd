/*
    PSPD 2023 - 1
    Alunos: Antonio Aldisio        202028211
            Fernando Miranda Calil 190106565
            Lorrany Oliveira Souza 180113992
*/


#include<stdio.h>
#include<mpi.h>

#define SLAVE 1
#define TAG 0
#define MASTER 0

int main(int argc, char * argv[]){
	int rank, size, len;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	int valor;
	int aux;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Get_processor_name(hostname, &len);
	if (rank == 0) {
		printf("Enviado o token para o rank %d \n", rank);
		MPI_Send(&valor, 1, MPI_INT, rank+1, TAG, MPI_COMM_WORLD);
		MPI_Recv(&valor, 1, MPI_INT, size-1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	else {
		 MPI_Recv(&aux, 1, MPI_INT, rank-1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		 printf("o rank [%d/ %s] recebeu o token %d \n", rank, hostname, valor);

		 if (rank == size-1)
			MPI_Send(&valor, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
		 else
			MPI_Send(&valor, 1, MPI_INT, rank+1, TAG, MPI_COMM_WORLD);
	}
	MPI_Finalize();

	return 0;
}