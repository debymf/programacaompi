#include <stdio.h>
#include "mpi.h"

int Ceiling_log2(int  x  /* in */) {
    unsigned temp = (unsigned) x - 1;
    int result = 0;

    while (temp != 0) {
         temp = temp >> 1;
         result = result + 1 ;
    }
    return result;
} /* Ceiling_log2 */

int I_receive(
        int   stage       /* in  */,
        int   my_rank     /* in  */, 
        int*  source_ptr  /* out */) {
    int   power_2_stage;

    /* 2^stage = 1 << stage */
    power_2_stage = 1 << stage;
    if ((power_2_stage <= my_rank) && 
            (my_rank < 2*power_2_stage)){
        *source_ptr = my_rank - power_2_stage;
        return 1;
    } else return 0;
} /* I_receive */

int I_send(
        int   stage     /* in  */,
        int   my_rank   /* in  */,
        int   p         /* in  */, 
        int*  dest_ptr  /* out */) {
    int power_2_stage;

    /* 2^stage = 1 << stage */
    power_2_stage = 1 << stage;  
    if (my_rank < power_2_stage){
        *dest_ptr = my_rank + power_2_stage;
        if (*dest_ptr >= p) return 0;
        else return 1;
    } else return 0;
} /* I_send */            

void Send(
        float  a     /* in */,
        float  b     /* in */, 
        int    n     /* in */, 
        int    dest  /* in */) {

    MPI_Send(&a, 1, MPI_FLOAT, dest, 0, MPI_COMM_WORLD);
    MPI_Send(&b, 1, MPI_FLOAT, dest, 1, MPI_COMM_WORLD);
    MPI_Send(&n, 1, MPI_INT, dest, 2, MPI_COMM_WORLD);
} /* Send */

void Receive(
        float*  a_ptr  /* out */, 
        float*  b_ptr  /* out */,
        int*    n_ptr  /* out */,
        int     source /* in  */) {

    MPI_Status status;

    MPI_Recv(a_ptr, 1, MPI_FLOAT, source, 0, 
        MPI_COMM_WORLD, &status);
    MPI_Recv(b_ptr, 1, MPI_FLOAT, source, 1, 
        MPI_COMM_WORLD, &status);
    MPI_Recv(n_ptr, 1, MPI_INT, source, 2, 
        MPI_COMM_WORLD, &status);
} /* Receive */

void Get_data1(
        float*  a_ptr    /* out */,
        float*  b_ptr    /* out */,
        int*    n_ptr    /* out */,
        int     my_rank  /* in  */, 
        int     p        /* in  */) {

    int source;
    int dest;
    int stage;

    int Ceiling_log2(int  x);
    int I_receive( int stage, int my_rank, int*  source_ptr);
    int I_send(int stage, int my_rank, int p, int* dest_ptr);
    void Send(float a, float b, int n, int dest);
    void Receive(float* a_ptr, float* b_ptr, int* n_ptr, int source);

    if (my_rank == 0){
        printf("Enter a, b, and n\n");
        scanf("%f %f %d", a_ptr, b_ptr, n_ptr);
    } 
    for (stage = 0; stage < Ceiling_log2(p); stage++)
        if (I_receive(stage, my_rank, &source))
            Receive(a_ptr, b_ptr, n_ptr, source);
        else if (I_send(stage, my_rank, p, &dest))
            Send(*a_ptr, *b_ptr, *n_ptr, dest);
} /* Get_data1*/

main(int argc, char** argv) {
    int         my_rank;   /* rank do processo atual   */
    int         p;         /* numero de processos  */
    float       a;   /* intervalo esquerdo  */
    float       b;   /* intervalo direito  */
    int         n;  /* numero de trapezios */
    float       h;         /* largura da base do trapezio */
    float       local_a;   /*intervalo esquerdo local*/
    float       local_b;   /* intervalo direito local*/
    int         local_n;   /* numero de trapezios local*/
    float       integral;  /* resultado da integral do processo */
    float       total;     /* resultado final da integral */
    int         source;    /* processo que esta enviando o resultado*/
    int         dest = 0;  /* o destino das msgs sera o processo 0*/
    int         tag = 0;
    MPI_Status  status;
    
  
    float Trap(float local_a, float local_b, int local_n,
              float h);    /* Calcula a integral local */

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    
    Get_data1(&a, &b, &n, my_rank, p);

    h = (b-a)/n;   
    local_n = n/p;
    
     
    local_a = a + my_rank*local_n*h;
    local_b = local_a + local_n*h;
    integral = Trap(local_a, local_b, local_n, h);
    
   
    
    if (my_rank == 0) {
        total = integral;
        for (source = 1; source < p; source++) {
            MPI_Recv(&integral, 1, MPI_FLOAT, source, tag,
                MPI_COMM_WORLD, &status);
            total = total + integral;
        }
    } else {  
        MPI_Send(&integral, 1, MPI_FLOAT, dest,
            tag, MPI_COMM_WORLD);
    }


    if (my_rank == 0) {
        printf("With n = %d trapezoids, our estimate\n",
            n);
        printf("of the integral from %f to %f = %f\n",
            a, b, total);
    }


    MPI_Finalize();
} /*  main  */


float Trap(
          float  local_a   /* in */,
          float  local_b   /* in */,
          int    local_n   /* in */,
          float  h         /* in */) {

    float integral;   
    float x;
    int i;

    float f(float x); 

    integral = (f(local_a) + f(local_b))/2.0;
    x = local_a;
    for (i = 1; i <= local_n-1; i++) {
        x = x + h;
        integral = integral + f(x);
    }
    integral = integral*h;
    return integral;
} /*  Trap  */


float f(float x) {
    float return_val;
    return_val = x*x;
    return return_val;
} /* f */


