//Name: Georges Junior Naddaf
//Project4-Serial_Quicksort

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

#define LENGTH 100

void QuickSortSerial(float *pf_output, float *pf_input, unsigned int ui_len);

void get_walltime(double* wcTime) {

	struct timeval tp;

	gettimeofday(&tp, NULL);

	*wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);

}


int main(int argc, char *argv[]) {
	
	double d_S, d_E;
	float *input = (float*)malloc(sizeof(float)*LENGTH);
	float *output = (float*)malloc(sizeof(float)*LENGTH);

	for(int i=LENGTH-1; i>=0; i--) {
		input[i]= LENGTH - 1 - i;
	}

	//printf("Input array: \n");

	//for(int i=0; i<LENGTH; i++) {
	//	printf("%f \n",input[i]);
	//}

	// start benchmark
	get_walltime(&d_S);

	QuickSortSerial(output,input,LENGTH);

	// end benchmark
	get_walltime(&d_E);
	
	//printf("Output array: \n");

	//for(int i=0; i<LENGTH; i++) {
	//	printf("%f \n",output[i]);
	//}
	
	free(input);
	free(output);
	
	printf("Total time = %f \n", d_E - d_S);

	return 0;

}

void swap(float* a, float* b) {
	float i = *a;
	*a = *b;
	*b = i;
}

int partition (float input[], int min, int max) {
	int index = input[max]; //Choose last element as pivot
	int b = (min - 1);

	for (int j = min; j <= max- 1; j++)
	{
		if (input[j] <= index)
		{
			b++;
			swap (&input[b], &input[j]);
		}
			
	}
	swap (&input[b + 1], &input[max]);
	return (b + 1);
}

void quickSort(float *output, int min, int max) {
	if (min < max)
	{        
		int index = partition(output, min, max); 
		quickSort(output, min, index - 1);  
		quickSort(output, index + 1, max);
	}

}

void QuickSortSerial(float *pf_output, float *pf_input, unsigned int ui_len) {

	int min = 0;
	int max = ui_len-1;

	quickSort(pf_input, min, max);

	for(int i=0; i<ui_len; i++) {
		pf_output[i]=pf_input[i];
	}

}

