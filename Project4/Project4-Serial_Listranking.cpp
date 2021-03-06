//Name: Georges Junior Naddaf
//Project4-ListRankSerial

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#define LENGTH 1000

void ListRankSerial(float *pf_output, float *pf_input, unsigned int ui_len);

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
	
	input[LENGTH-1] = -1;

	//printf("Input array: \n");

	//for(int i=0; i<LENGTH; i++) {
	//	printf("%f \n",input[i]);
	//}

	// start benchmark
	get_walltime(&d_S);

	ListRankSerial(output,input,LENGTH);

	// end benchmark
	get_walltime(&d_E);
	
	//printf("Output array: \n");

	//for(int i=0; i<LENGTH; i++) {
	//	printf("%f \n",output[i]);
	//}
	
	printf("Total time = %f \n", d_E - d_S);
	
	free(input);
	free(output);

	return 0;

}

void ListRankSerial(float *pf_output, float *pf_input, unsigned int ui_len) {

	int *Q = (int*)malloc(sizeof(int)*LENGTH); //Needs to be int as it is used as array subscript later
	for(int i=0; i<ui_len; i++) {
		if(pf_input[i] != -1)
			pf_output[i]=1;
		else 
			pf_output[i] = 0;
	}

	for(int i=0; i<ui_len; i++) {
		Q[i] = (int)pf_input[i];
	}

	for(int i=0; i<ui_len; i++) {

		while(Q[i]!=-1 && Q[Q[i]]!=-1) {
			pf_output[i] += pf_output[Q[i]];
			Q[i] = Q[Q[i]];
			if(Q[i] == Q[Q[i]]) //Prevent endless loops
				break;
		}
	}


}

