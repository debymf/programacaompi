#include <stdio.h>
#include "mpi.h"
#include <time.h>
#define MAX_ORDER 100

typedef float LOCAL_MATRIX_T[MAX_ORDER][MAX_ORDER];

void Print_matrix(LOCAL_MATRIX_T local_A, int local_m, int n, int my_rank, int p) {
	int   i, j;
	float temp[MAX_ORDER][MAX_ORDER];

	MPI_Gather(local_A, local_m*MAX_ORDER, MPI_FLOAT, temp, 
		local_m*MAX_ORDER, MPI_FLOAT, 0, MPI_COMM_WORLD);

	if (my_rank == 0) {
		for (i = 0; i < p*local_m; i++) {
			for (j = 0; j < n; j++){
				printf("%4.1f ", temp[i][j]);
			}
		printf("\n");
		}
	} 
} 

void Print_vector(float local_y[], int local_m, int my_rank, int p) {
	int   i;
	float temp[MAX_ORDER];

	MPI_Gather(local_y, local_m, MPI_FLOAT, temp, local_m, MPI_FLOAT,0, MPI_COMM_WORLD);
	if (my_rank == 0) {
		for (i = 0; i < p*local_m; i++){
			printf("%4.1f ", temp[i]);
			printf("\n");
		}
	} 
}

void Generate_matrix(LOCAL_MATRIX_T  local_A, int local_m, int  n, int my_rank, int p) {
	int i, j;
	unsigned int iseed = (unsigned int)time(NULL);
	srand (iseed);
	LOCAL_MATRIX_T temp;

	/* O processo 0 preenche a matriz */
	if (my_rank == 0) {
		for (i = 0; i < p*local_m; i++){ 
			for (j = 0; j < n; j++){
				temp[i][j] = (rand ()%100)/(rand ()%50);
			}
		}
	}
	
	MPI_Scatter(temp, local_m*MAX_ORDER, MPI_FLOAT, local_A,local_m*MAX_ORDER, MPI_FLOAT, 0, MPI_COMM_WORLD);
} 

void Generate_vector(float  local_x[], int local_n, int my_rank, int p) {
	int   i;
	float temp[MAX_ORDER];

	/* O processo 0 preenche o vetor*/
	if (my_rank == 0) {
		for (i = 0; i < p*local_n; i++) 
		temp[i]=(rand ()%100)/(rand ()%50);
	}

	MPI_Scatter(temp, local_n, MPI_FLOAT, local_x, local_n, MPI_FLOAT,0, MPI_COMM_WORLD);
}  

int main(int argc, char* argv[]) {
	int             my_rank;
	int             p;
	LOCAL_MATRIX_T  local_A; 
	float           global_x[MAX_ORDER];
	float           local_x[MAX_ORDER];
	float           local_y[MAX_ORDER];
	int             m, n;
	int             local_m, local_n;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	if (my_rank == 0) {
		printf("Escreva a ordem da matriz (m x n)\n");
		scanf("%d %d", &m, &n);
	}
	
	MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

	local_m = m/p;
	local_n = n/p;
	Generate_matrix(local_A, local_m, n, my_rank, p);
	Generate_vector(local_x, local_n, my_rank, p);
	
	Print_matrix(local_A, local_m, n, my_rank, p);
	Print_vector(local_x, local_n, my_rank, p);

	MPI_Finalize();

	return 0;
}