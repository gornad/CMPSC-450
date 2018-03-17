// compiles with:
// g++ slow_code.cpp -o slow_code


#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

int i_R = 1000;	
int i_N = 100;

void get_walltime(double* wcTime) {

     struct timeval tp;

     gettimeofday(&tp, NULL);

     *wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);

}

// Complex algorithm for evaluation
// Optimizations:
// - Switched loop nest structure for maximum loop unrolling
// - Simplified trigonometry formula: sin^2(x) - cos^2(x) = -(cos^2(x) - sin^2(x)) = - cos(2x)
// - Replaced fmod with % as expression should return integer (as i_v contains i*i values and i is an integer)
// - Precomputed -cos(2*d_val) outside the inner loop and stored in d_val
// - No need to compute the size() of the vectors; declared i_N and I_R as global variables and use anywhere


void myfunc(std::vector<std::vector<double> > &v_s, 
	std::vector<std::vector<double> > &v_mat, std::vector<int> &i_v)
{
	// this assumes that the two dimensional vector is square 

	double d_val;
	//double bs = 500, nb = i_N/bs;
	
	//for(int k=1; k<= nb; k++) {

		for (int i = 0; i < i_N; i++)
		{
			d_val = -cos(2*(i_v[i]%256)); // this should return an integer
            //double begin = (k-1)*bs+1;
            //double end = k*bs;
			for (int j = 0 /*begin*/ ; j <= i_N /*end*/ ; j++)
			{
				//d_val = round(fmod(i_v[i],256)); // this should return an integer

				v_mat[i][j] = v_s[i][j]*d_val;

				/*v_mat[i][j+1] = v_s[i][j+1]*(sin(d_val)*sin(d_val)-cos(d_val)*cos(d_val));
				v_mat[i][j+2] = v_s[i][j+2]*(sin(d_val)*sin(d_val)-cos(d_val)*cos(d_val));
				v_mat[i][j+3] = v_s[i][j+3]*(sin(d_val)*sin(d_val)-cos(d_val)*cos(d_val));
				v_mat[i][j+4] = v_s[i][j+4]*(sin(d_val)*sin(d_val)-cos(d_val)*cos(d_val));
				v_mat[i][j+5] = v_s[i][j+5]*(sin(d_val)*sin(d_val)-cos(d_val)*cos(d_val));
				v_mat[i][j+6] = v_s[i][j+6]*(sin(d_val)*sin(d_val)-cos(d_val)*cos(d_val));
				v_mat[i][j+7] = v_s[i][j+7]*(sin(d_val)*sin(d_val)-cos(d_val)*cos(d_val));
				v_mat[i][j+8] = v_s[i][j+8]*(sin(d_val)*sin(d_val)-cos(d_val)*cos(d_val));
				v_mat[i][j+9] = v_s[i][j+9]*(sin(d_val)*sin(d_val)-cos(d_val)*cos(d_val));*/
			}
		}
	//}
}

int main(int argc, char *argv[])
{

	// this should be called as> ./slow_code <i_R> <i_N>

	double d_S, d_E;

	// parse input parameters 
	if (argc >= 2)
	{
		i_R = atoi(argv[1]);
	}

	if (argc >= 3)
	{
		i_N = atoi(argv[2]);
	}

	// some declarations
	std::vector<std::vector<double> > vd_s(i_N, std::vector<double>(i_N) );
	std::vector<std::vector<double> > vd_mat(i_N, std::vector<double>(i_N) );
	std::vector<int> vi_v(i_N);

	// populate memory with some random data
	for (int i = 0; i < i_N; i++)
	{
		vi_v[i] = i * i;
		for (int j = 0; j < i_N; j++)
			vd_s[i][j] = j + i;
	}

	// start benchmark
	get_walltime(&d_S);

	// iterative test loop
	for (int i = 0; i < i_R; i++)
	{
		myfunc(vd_s, vd_mat, vi_v);
	}

	// end benchmark
	get_walltime(&d_E);

	// report results
	printf("Elapsed time: %f\n", d_E - d_S);

	return 0;
}
