#include <stdio.h>
#include "mpi.h"
#include <time.h>

void Sum(float *local_num, int my_rank, int p, int root){
	int tag=0;
	int i;
	int source;
	float temp;
	
	MPI_Status  status;   
	
	if (my_rank!=root){
		  MPI_Send(local_num, 4, MPI_FLOAT, 0, tag, MPI_COMM_WORLD);
	}
	else{	
		printf("%f ", *local_num);
		for (i = 0; i < p; i++) {
			/* A variavel temp armazena os valores intermediarios de soma*/
			if (i!=root){
				temp = *local_num;
				MPI_Recv(local_num, 4, MPI_FLOAT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
				printf("+ %f ", *local_num);
				*local_num = *local_num + temp;
			}
		}
		printf("= %f\n", *local_num);
	}
	
}

int main(int argc, char* argv[]) {
	int             my_rank;
	int             p;
	float 		local_num;
	float           global_sum;
	unsigned int iseed = (unsigned int)time(NULL);
	int root = 0;
	

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Gera um número aleatório para cada processo */
	srand (iseed);
	local_num = (rand ()%100);
	
	Sum(&local_num, my_rank,p,root);
	
	MPI_Finalize();

	return 0;
}