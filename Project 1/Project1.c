#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "Project1.h"

int main(void *arg) {
    
    const long N = 100;
    const long R = 100;

    double *A = (double*) malloc(N*sizeof(double));
    double *B = (double*) malloc(N*sizeof(double));
    double *C = (double*) malloc(N*sizeof(double));
    double *D = (double*) malloc(N*sizeof(double));

    double *S = (double*) malloc(sizeof(double));
    double *E = (double*) malloc(sizeof(double));
    double MFLOPS;

    for (int i = 0; i < N; i++) {
        A[i] = 0;
        B[i] = 1;
        C[i] = 2;
        D[i] = 3;
    }

    get_walltime(S);  // get start time
    for(int j=0; j<R; j++) {
        for(int i=0; i<N; i++)
		A[i] = B[i] + C[i] * D[i];

        if(A[2] < 0)
	    dummy(A,B,C,D);
    }


    get_walltime(E); // get end time stamp

    MFLOPS = (R*N*2)/((*E-*S)*1000000); // calculate MFLOPS

    printf("MFLOPS: %f \n", MFLOPS);

    free(A);
    free(B);
    free(C);
    free(D);

    free(S);
    free(E);
}

void get_walltime(double* wcTime) {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    *wcTime = (double)(tp.tv_sec + tp.tv_usec / 1000000.0);
}

void dummy(double* A, double* B, double* C, double* D) {
}

