#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>


int R = 1000;	
int N = 100000;
int M = 256;

void get_walltime(double* wcTime) {

     struct timeval tp;

     gettimeofday(&tp, NULL);

     *wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);

}

// complex algorithm for evaluation

void benchFunc(float *A, float *B, int N, int M) {

	for (int i = 0; i < N-M; i+=10) {
		
		B[i] = 0;
	      	B[i+1] = 0;
	      	B[i+2] = 0;
	      	B[i+3] = 0;
	      	B[i+4] = 0;
	      	B[i+5] = 0;
	      	B[i+6] = 0;
	      	B[i+7] = 0;
	      	B[i+8] = 0;
	      	B[i+9] = 0;

		for (int j = 0; j < M; j++) {
			B[i] += A[i+j];
			B[i+1] += A[i+1+j];
			B[i+2] += A[i+2+j];
			B[i+3] += A[i+3+j];
			B[i+4] += A[i+4+j];
			B[i+5] += A[i+5+j];
			B[i+6] += A[i+6+j];
			B[i+7] += A[i+7+j];
			B[i+8] += A[i+8+j];
			B[i+9] += A[i+9+j];

		}
	      	B[i] /= M;
	      	B[i+1] /= M;
	      	B[i+2] /= M;
	      	B[i+3] /= M;
	      	B[i+4] /= M;
	      	B[i+5] /= M;
	      	B[i+6] /= M;
	      	B[i+7] /= M;
	      	B[i+8] /= M;
	      	B[i+9] /= M;
	
	}

}
int main(int argc, char *argv[])
{

	double d_S, d_E;

	// parse input parameters 
	if (argc >= 2)
	{
		R = atoi(argv[1]);
	}

	if (argc >= 3)
	{
		N = atoi(argv[2]);
	}

	// some declarations
	float *A = (float*) malloc(N*sizeof(float));
        float *B = (float*) malloc(N*sizeof(float));

	// start benchmark
	get_walltime(&d_S);

	// iterative test loop
	for (int i = 0; i < R; i++)
	{
		benchFunc(A, B, N, M);
	}

	// end benchmark
	get_walltime(&d_E);

	// report results
	printf("Elapsed time: %f\n", d_E - d_S);

	return 0;
}
