#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpi.h"

/** Multiply two matrices a(of size mxk) and b(of size kxn), and add the result to c(of size mxn)
 */
void multiplyMatrix(
    double *a,
    double *b,
    double *c,
    int m,
    int k,
    int n);

/** Create a matrix of size nrowsxncols.
 *  If init is set to "random", elements are initialized randomly. If it is "zero", elements are initialized to zero.
 */
void createMatrix(
    double **pmat,
    int nrows,
    int ncols,
    char *init);
	
/* Parallel SUMMA matrix-matrix multiplication of two matrices of size NxN
 */
void summa(int N)
{
    int rank, p;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    p = (int)sqrt(p);

    /* Create communicators */
    int row_color = rank / p;
    MPI_Comm row_comm;
    MPI_Comm_split(MPI_COMM_WORLD, row_color, rank, &row_comm);

    int col_color = rank % p + p;
    MPI_Comm col_comm;
    MPI_Comm_split(MPI_COMM_WORLD, col_color, rank, &col_comm);

    /* create matrices */
    double * Aloc, * Bloc, * Cloc;
    createMatrix(&Aloc, N / p, N / p, "random");
    createMatrix(&Bloc, N / p, N / p, "random");
    createMatrix(&Cloc, N / p, N / p, "zero");

    double * Atemp, * Btemp;
    createMatrix(&Atemp, N / p, N / p, "zero");
    createMatrix(&Btemp, N / p, N / p, "zero");

    int size = N * N / p / p;

    /* compute */
    for (int k = 0; k < p; ++k) {
        if (col_color == k + p)
            memcpy(Atemp, Aloc, size);

        if (row_color == k)
            memcpy(Btemp, Bloc, size);

        MPI_Bcast(Atemp, size, MPI_DOUBLE, k, row_comm);
        MPI_Bcast(Btemp, size, MPI_DOUBLE, k, col_comm);

        multiplyMatrix(Atemp, Btemp, Cloc, N / p, N / p, N / p);
    }

    /* cleanup */
    MPI_Comm_free(&row_comm);
    MPI_Comm_free(&col_comm);
    free(Aloc);
    free(Bloc);
    free(Cloc);
    free(Atemp);
    free(Btemp);
}

void createMatrix(
    double **pmat,
    int nrows,
    int ncols,
    char *init)
{
  double *mat;
  int i;
  *pmat = mat = (double *)malloc(nrows * ncols * sizeof(mat[0]));
  if (strcmp(init, "random") == 0) { // Initialize the matrix elements randomly
    srand(time(0));
    for (i = 0; i < nrows * ncols; i++) { mat[i] = rand() / (double)RAND_MAX; }
  } else { // Set all matrix elements to zero
    memset(mat, 0, nrows * ncols * sizeof(mat[0]));
  }
}

/** Multiply two matrices a(of size mxk) and b(of size kxn), and add the result to c(of size mxn)
 */
void multiplyMatrix(
    double *a,
    double *b,
    double *c,
    int m,
    int k,
    int n)
{
  int im, ik, in;
  for (im = 0; im < m; im++) {
    for (in = 0; in < n; in++) {
      for (ik = 0; ik < k; ik++) {
        c[im + in * m] += a[im + ik * m] * b[ik + in * k];
      }
    }
  }
}

void printUsage()
{
  printf("Usage: mpirun -np [num-procs] ./summa N\n");
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int procRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
  if (argc < 2 && procRank == 0) {
    printUsage();
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  int N = atoi(argv[1]);
  double start = MPI_Wtime();
  summa(N);
  double end = MPI_Wtime();
  MPI_Finalize();
  if(procRank == 0)
	printf("\nTime: %.4f seconds\n", (end - start));
  return 0;
}
