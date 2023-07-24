# Projeto Final

## Alunos

|Matrícula | Aluno |
| -- | -- |
|  18/0113992  | Lorrany de Oliveira |
|  20/2028211  | Antônio Aldísio     |
|  19/0106565  | Fernando Calli      |
## Como rodar?

### Versão MPI
- mpicc -o jogodavida jogodavidampi.c
- mpirun -np 4 ./jogodavida



### Versão OpenMP

- gcc -o omp jogodavidaomp.c -fopenmp
- OMP_NUM_THREADS=4 ./omp


### Versão MPI + OpenMP

- mpicc -fopenmp -o executavel jogodavidampi_omp.c
- mpirun -np 4 ./executavel

### Versão Spark

- python jogodavida.py  POWMIN POWMAX