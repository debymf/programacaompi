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

void Matrix_matrix(LOCAL_MATRIX_T  local_A,LOCAL_MATRIX_T  local_B,LOCAL_MATRIX_T  resul, int m,int n1,int n2,int local_m1,int local_n1, int local_m2, int local_n2, int my_rank) {
	int x,k,i, j;
	LOCAL_MATRIX_T global_B;

	/*Obtemos todos os valores de B e colocamos em global_B*/
	for(k=0; k<local_m2; k++){ 
		MPI_Allgather(local_B[k], m, MPI_FLOAT, global_B[k], m, MPI_FLOAT, MPI_COMM_WORLD);	
	}
    
	for (i = 0; i < local_m2; i++) {
		for (j=0;j<n2; j++){
			resul[i][j] = 0.0;
			for (k = 0; k < n2; k++){
			resul[i][j] = resul[i][j] + local_A[i][k]*global_B[k][j];
			}
		}
		
	}
		
}

int main(int argc, char* argv[]) {
	int             my_rank;
	int             p;
	LOCAL_MATRIX_T  local_A; 
	LOCAL_MATRIX_T  local_B;
	LOCAL_MATRIX_T  resul;
	int             m, n1,n2;
	int             local_m1, local_n1, local_m2, local_n2;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	if (my_rank == 0) {
		printf("Escreva a ordem da matriz 1 (m x n)\n");
		scanf("%d %d", &m, &n1);
		printf("Escreva a ordem da matriz 2(somente colunas) (n)\n");
		scanf("%d", &n2);
	}
	
	MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&n1, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&n2, 1, MPI_INT, 0, MPI_COMM_WORLD);

	local_m1 = m/p;
	local_n1 = n1/p;
	local_m2 = n1/p;
	local_n2 = n2/p;
	
	Generate_matrix(local_A, local_m1, n1, my_rank, p);
	Generate_matrix(local_B, local_m2, n2, my_rank, p);
	if (my_rank==0)
		printf("Matriz A:\n");
	Print_matrix(local_A, local_n1, n1, my_rank, p);
	if (my_rank==0)
		printf("Matriz B:\n");
	Print_matrix(local_B, local_n2, n2, my_rank, p);
	
	Matrix_matrix(local_A,local_B,resul, m,n1,n2,local_m1,local_n1,local_m2,local_n2,my_rank);
	
	if (my_rank==0)
		printf("Matriz Resultante:\n");
	Print_matrix(resul, local_m1, n2, my_rank, p);

	MPI_Finalize();

	return 0;
}