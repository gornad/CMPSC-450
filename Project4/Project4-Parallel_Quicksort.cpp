//Name: Georges Junior Naddaf
//Project4-Parallel_Quicksort

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define LENGTH 200000
#define NUM_THREADS 1000

void QuickSortParallel(float *pf_output, float *pf_input, unsigned int ui_len);

void get_walltime(double* wcTime) {

	struct timeval tp;

	gettimeofday(&tp, NULL);

	*wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);

}


int main(int argc, char *argv[]) {
	
	double d_S, d_E;
	float *input = (float*)malloc(sizeof(float)*LENGTH);
	float *output = (float*)malloc(sizeof(float)*LENGTH);
	bool check = true;

	for(int i=LENGTH-1; i>=0; i--) {
		input[i]= LENGTH - 1 - i;
	}

	//printf("Input array: \n");

	//for(int i=0; i<LENGTH; i++) {
	//	printf("%f \n",input[i]);
	//}

	// start benchmark
	get_walltime(&d_S);

	QuickSortParallel(output,input,LENGTH);

	// end benchmark
	get_walltime(&d_E);
	
	//printf("Output array: \n");

	//for(int i=0; i<LENGTH; i++) {
	//	printf("%f \n",output[i]);
		//if(output[i+1]!=output[i]+1)
		//	check = false;
	//}
	
	/*if(check)
		printf("TRUE\n");
	else
		printf("FALSE\n");*/

	printf("Total time = %f \n", d_E - d_S);
	
	free(input);
	free(output);

	return 0;

}

void swap(float* a, float* b) {
	float i = *a;
	*a = *b;
	*b = i;
}

int partition (float input[], int min, int max) {
	int minimum = min;
	int maximum = max;
	int index = input[maximum]; //Choose last element as pivot
	int b = (min - 1);

	for (int j = minimum; j <= maximum- 1; j++)
	{
		if (input[j] <= index)
		{
			b++;
			swap (&input[b], &input[j]);
		}
			
	}
	swap (&input[b + 1], &input[maximum]);
	return (b + 1);
}

/*void quickSort(float output[], int low, int high) {  //Sections
	if (low < high)
	{        
		int index = partition(output, low, high); 

		#pragma omp parallel num_threads(NUM_THREADS)
		{
			#pragma omp sections 
			{
				#pragma omp section
				quickSort(output, low, index - 1);  

				#pragma omp section
				quickSort(output, index + 1, high);
			}
		}
	}

}*/

void quickSort(float output[], int low, int high) { //Tasks 
	if (low < high)
	{        
		int index = partition(output, low, high); 

		#pragma omp task
		quickSort(output, low, index - 1);  

		#pragma omp task
		quickSort(output, index + 1, high);
	}

}

/*void QuickSortParallel(float *pf_output, float *pf_input, unsigned int ui_len) { //Quicksort using sections

	int min = 0;
	int max = ui_len-1;

	quickSort(pf_input, min, max);

	for(int i=0; i<ui_len; i++) {
		pf_output[i]=pf_input[i];
	}

}*/

void QuickSortParallel(float *pf_output, float *pf_input, unsigned int ui_len) { //Quicksort using tasks

	int min = 0;
	int max = ui_len-1;
	#pragma omp parallel num_threads(NUM_THREADS)
	#pragma omp single
	quickSort(pf_input, min, max);
	#pragma omp taskwait
	for(int i=0; i<ui_len; i++) {
		pf_output[i]=pf_input[i];
	}

}

