#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>


int R = 10000;	
int N = 100;

void get_walltime(double* wcTime) {

     struct timeval tp;

     gettimeofday(&tp, NULL);

     *wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);

}

// complex algorithm for evaluation
void myfunc(std::vector<std::vector<int> > &a, std::vector<int> &b, std::vector<int> &c)
{
	// this assumes that the two dimensional vector is square 
	double bs = 100, nb = N/bs;
	
	for(int k=1; k<= nb; k++) {

        double begin = (k-1)*bs+1;
        double end = k*bs;

		for (int i = 0; i < N; i+=5)
		{
			for (int j = begin; j <= end; j++)
			{
			
				c[i] = c[i] + a[i][j] * b[j];
				c[i+1] = c[i+1] + a[i+1][j] * b[j];
				c[i+2] = c[i+2] + a[i+2][j] * b[j];
				c[i+3] = c[i+3] + a[i+3][j] * b[j];
				c[i+4] = c[i+4] + a[i+4][j] * b[j];
				//c[i+5] = c[i+5] + a[i+5][j] * b[j];
				//c[i+6] = c[i+6] + a[i+6][j] * b[j];
				//c[i+7] = c[i+7] + a[i+7][j] * b[j];
				//c[i+8] = c[i+8] + a[i+8][j] * b[j];
				//c[i+9] = c[i+9] + a[i+9][j] * b[j];
			}
		}
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
	std::vector<std::vector<int> > a(N, std::vector<int>(N) );
	std::vector<int> b(N);
	std::vector<int> c(N);

	// populate memory with some random data
	for (int i = 0; i < N; i++)
	{
		b[i] = i * i;
		c[i] = i * i;
		for (int j = 0; j < N; j++)
			a[i][j] = j + i;
	}

	// start benchmark
	get_walltime(&d_S);

	// iterative test loop
	for (int i = 0; i < R; i++)
	{
		myfunc(a, b, c);
	}

	// end benchmark
	get_walltime(&d_E);

	// report results
	printf("Elapsed time: %f\n", d_E - d_S);

	return 0;
}
