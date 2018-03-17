//Name: Georges Junior Naddaf
//Project3-partition.cpp

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define NUM_THREADS 1
#define LENGTH 1048526	
//Input size of array to be summed (2^17 = 131072)

double fast_sum(double *pd_input, int i_N); //A 2D array input is NEEDED. A 1D array approach similar to the binary summation example in the slides will not work. Prove me wrong.


void get_walltime(double* wcTime) {

     struct timeval tp;

     gettimeofday(&tp, NULL);

     *wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);

}


int main(int argc, char *argv[]) {

	double d_S, d_E;
	double arr_sum[LENGTH];
	double sum_of_array;

	// start benchmark
	get_walltime(&d_S);

	sum_of_array = fast_sum(arr_sum,LENGTH);

	// end benchmark
	get_walltime(&d_E);

	printf("SUM = %f\n", sum_of_array);
	printf("Elapsed time: %f\n", d_E - d_S);
}

// Code was heavily inspired by "summation in openmp" example in slides
double fast_sum(double *pd_input, int i_N) {

	int i_numThreads;

	double sumofarray = 0;

	for(int i=0; i<i_N; i++)
		pd_input[i] = 1;

	omp_set_num_threads(NUM_THREADS);


	#pragma omp parallel for reduction(+:sumofarray) 
		for (int i = 0; i < i_N; i++)
		{
			sumofarray+=pd_input[i];
		}

	return sumofarray;
}



