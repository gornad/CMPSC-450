//Name: Georges Junior Naddaf
//Project3-binary.cpp

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define NUM_THREADS 1
#define LENGTH 1048526	
//Input size of array to be summed (2^17 = 131072)

double fast_sum(double **pd_input, int i_N); 

void get_walltime(double* wcTime) {

     struct timeval tp;

     gettimeofday(&tp, NULL);

     *wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);

}


int main(int argc, char *argv[]) {

	double d_S, d_E;
	double* arr_sum[LENGTH];
	double sum_of_array;
	int i_N_log = log2(LENGTH);

	// start benchmark
	get_walltime(&d_S);

	sum_of_array = fast_sum(arr_sum,LENGTH);

	// end benchmark
	get_walltime(&d_E);

	printf("SUM = %f\n", sum_of_array);
	printf("Elapsed time: %f\n", d_E - d_S);
}

double fast_sum(double **pd_input, int i_N) {

	int i_numThreads;
	int i_N_log = log2(i_N);

	//Allocate 2D array
	for(int i=0; i<i_N; i++)
		pd_input[i]= (double*)malloc((i_N_log+1)*sizeof(double));

	// Populate with data
	for(int i=0; i<i_N; i++)
		for(int j=0; j<(i_N_log+1); j++)
			pd_input[i][j] = 1;

	omp_set_num_threads(NUM_THREADS);

	int h;

	
	for(h = 1; h<=i_N_log; h++) {

		#pragma omp parallel
		{
			// declare some private variables
			int i, i_nthreads, i_ID;
			double temp1;

			// this will range from 0 to nThreads
			i_ID = omp_get_thread_num();

			// get total number of threads since 
			// we aren't guaranteed what we request
			i_nthreads = omp_get_num_threads();
			temp1 = (i_N/(pow(2,h)));

			// limit writing to numThreads to a single thread
			if (i_ID == 0) i_numThreads = i_nthreads;

			for (i = i_ID; i < temp1; i+=i_nthreads)
			{
				pd_input[i][h+1] = pd_input[2*i][h] + pd_input[2*i+1][h];
				//printf("ID: %d temp1: %f i: %d arr[i]: %d\n",i_ID,temp1,i,pd_input[i][h+1]);
			}
		}
	}
	
	double sumofarray = pd_input[0][i_N_log+1];
	
	//for(int i=0; i<i_N; i++)
	//	free(pd_input[i]); 
	//Freeing memory crashes code when input is 4,16,64....
	//It seems tp be attempting to free memory that was not malloc'ed and I was not able to fix.

	return sumofarray;
}



