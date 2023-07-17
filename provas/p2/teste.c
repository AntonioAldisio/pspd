#include<stdio.h>
#include <omp.h>
#include <string.h>
#define MAX 10


int main (int argc , char *argv[]){
	int i =0;
	#pragma omp parrallel private(i)
	{
		if(omp_get_thread_num() == 1)
			i=i+10;
	}
	printf("i %d\n", i);
//	#pragma omp parrallel
//	{
//		int soma =0;
//		#pragma omp for
//		for (int i =0; i<MAX; i++){
//			soma +=omp_get_num_threads()*i;
//		}
//		printf("[%d] interrou %d \n", omp_get_thread_num(), soma);
//	}
	return 0;
}
