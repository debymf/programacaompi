#include <stdio.h>
#include "mpi.h"
#include <time.h>
#define MAX_ORDER 100

void Sum_vector(float vec[], int size, int my_rank, int p, int root){
	int tag=0;
	int i,j;
	int source;
	float temp;
	
	MPI_Status  status;   
	
	if (my_rank!=0){
		for (j=0;j<size;j++){
			MPI_Send(&vec[j], sizeof(vec), MPI_FLOAT, 0,j, MPI_COMM_WORLD);
		}
	}
	else{	
		for (i = 0; i < p; i++) {
			if (root!= i){
				for (j=0;j<size;j++){
					/* temporario que salva as somas intermediarias dos vetores*/
					temp = vec[j];
					MPI_Recv(&vec[j], sizeof(vec), MPI_FLOAT, MPI_ANY_SOURCE, j, MPI_COMM_WORLD, &status);
					vec[j] = vec[j] + temp;
				}
			}
		}
	}
	
}

int main(int argc, char* argv[]) {
	int             my_rank;
	float     	local_vec[MAX_ORDER];
	int             p;
	int 		i;
	int 		size;
	float           global_sum;
	int 		root=0;
	unsigned int iseed = (unsigned int)time(NULL);
	
	/* Gera um numero aleatorio para o tamanho dos vetores */
	srand (iseed);
	size = (rand ()%20);
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Preenche o vetor com números aleatorios */
	srand (iseed);

	for(i=0;i<size;i++){
		local_vec[i] = ((rand ())%100);
		
	}
	Sum_vector(local_vec,size, my_rank,p, root);
	
	if (my_rank==0){
		for(i=0;i<size;i++){
			printf("vetor_soma[%d] = %f\n", i,local_vec[i]);
			
		}
	}
	MPI_Finalize();

	return 0;
}