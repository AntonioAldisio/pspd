 /*
    Como compilar:
    $ gcc -o omp jogodavidaomp.c -fopenmp

    Como rodar:
    $ OMP_NUM_THREADS=4 ./omp
*/
#include <stdio.h>
#include <omp.h>

int main(){
	int myid, nthreads;

	#pragma omp parallel private(myid, nthreads)
	{
	myid = omp_get_thread_num();
	#pragma omp single
	nthreads = omp_get_num_threads();
	printf("%d of %d - hello world!\n", myid, nthreads);
	}
	return 0;
}
