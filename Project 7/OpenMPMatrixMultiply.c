/******************************************************************************
* FILE: omp_mm.c
* DESCRIPTION:  
*   OpenMp Example - Matrix Multiply - C Version
*   Demonstrates a matrix multiply using OpenMP. Threads share row iterations
*   according to a predefined chunk size.
* AUTHOR: Blaise Barney
* LAST REVISED: 06/28/05
******************************************************************************/
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define NRA 1                 /* number of rows in matrix A */
#define NCA 1                 /* number of columns in matrix A */
#define NCB 1                 /* number of columns in matrix B */
#define NUM_THREADS 1000

void get_walltime(double* wcTime) {

	struct timeval tp;

	gettimeofday(&tp, NULL);

	*wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);

}

int main (int argc, char *argv[]) 
{
int	tid, nthreads, i, j, k, chunk;

double *a[NRA];
for (i=0; i<NRA; i++)
	 a[i] = (double *)malloc(NCA * sizeof(double));		/* matrix A to be multiplied */
 
double *b[NCA];
for (i=0; i<NCA; i++)
	 b[i] = (double *)malloc(NCB * sizeof(double));		/* matrix B to be multiplied */
 
double *c[NRA];
for (i=0; i<NRA; i++)
	 c[i] = (double *)malloc(NCB * sizeof(double));		/* result matrix C */

double d_S, d_E;
chunk = 100;                    /* set loop iteration chunk size */

/*** Spawn a parallel region explicitly scoping all variables ***/
#pragma omp parallel shared(a,b,c,nthreads,chunk) private(tid,i,j,k) num_threads(NUM_THREADS)
  {
  tid = omp_get_thread_num();
  if (tid == 0)
    {
    nthreads = omp_get_num_threads();
    printf("Starting matrix multiple example with %d threads\n",nthreads);
    printf("Initializing matrices...\n");
    }
  /*** Initialize matrices ***/
  #pragma omp for schedule (static, chunk) 
  for (i=0; i<NRA; i++)
    for (j=0; j<NCA; j++)
      a[i][j]= i+j;
  #pragma omp for schedule (static, chunk)
  for (i=0; i<NCA; i++)
    for (j=0; j<NCB; j++)
      b[i][j]= i*j;
  #pragma omp for schedule (static, chunk)
  for (i=0; i<NRA; i++)
    for (j=0; j<NCB; j++)
      c[i][j]= 0;

  /*** Do matrix multiply sharing iterations on outer loop ***/
  /*** Display who does which iterations for demonstration purposes ***/
  printf("Thread %d starting matrix multiply...\n",tid);
  get_walltime(&d_S);
  #pragma omp for schedule (static, chunk)
  for (i=0; i<NRA; i++)    
    {
    printf("Thread=%d did row=%d\n",tid,i);
    for(j=0; j<NCB; j++)       
      for (k=0; k<NCA; k++)
        c[i][j] += a[i][k] * b[k][j];
    }
  }   /*** End of parallel region ***/
  get_walltime(&d_E);
/*** Print results ***/
printf("******************************************************\n");
printf("Result Matrix:\n");
for (i=0; i<NRA; i++)
  {
  for (j=0; j<NCB; j++) 
    printf("%6.2f   ", c[i][j]);
  printf("\n"); 
  }
printf("******************************************************\n");
printf ("Done.\n");
printf("Total time = %f \n", d_E - d_S);

for (i=0; i<NRA; i++)
	 free(a[i]);
 
for (i=0; i<NCA; i++)
	 free(b[i]);
 
for (i=0; i<NRA; i++)
	 free(c[i]);
}

