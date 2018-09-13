#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define N 523216
#define B 1024
#define NUM_BANKS 16
#define LOG_NUM_BANKS 4
#define CONFLICT_FREE_OFFSET(n) \
    ((n) >> NUM_BANKS + (n) >> (2 * LOG_NUM_BANKS))
	

__global__ void scan(double *g_odata, double *g_idata, int n);
__global__ void prescan(double *g_odata, double *g_idata, double *blocksum, int n, int sharedmemory, int maxblocksize);
__global__ void gpusummation(double *g_odata, double *g_idata, double *blocksum, int n);

void bestscan(double *g_odata, double *g_idata, int n);
void scanCPU(double *f_out, double *f_in, int i_n);

double myDiffTime(struct timeval &start, struct timeval &end)
{
	double d_start, d_end;
	d_start = (double)(start.tv_sec + start.tv_usec/1000000.0);
	d_end = (double)(end.tv_sec + end.tv_usec/1000000.0);
	return (d_end - d_start);
}

int main() 
{
	double a[N], c[N], g[N];
	timeval start, end;

	double *dev_a, *dev_g;
	int size = N * sizeof(double);
	
	double d_gpuTime, d_cpuTime;

	// initialize matrices a 
	for (int i = 0; i < N; i++)
	{
		a[i] = (double)(rand() % 1000000) / 1000.0;
		//a[i] = 1;
		//printf("a[%i] = %f\n", i, a[i]);
	}
	// initialize a and b matrices here
	cudaMalloc((void **) &dev_a, size);
	cudaMalloc((void **) &dev_g, size);

	
	gettimeofday(&start, NULL);

	cudaMemcpy(dev_a, a, size, cudaMemcpyHostToDevice);

	bestscan(dev_g, dev_a, N);
	cudaDeviceSynchronize();

	cudaMemcpy(g, dev_g, size, cudaMemcpyDeviceToHost);

	gettimeofday(&end, NULL);
	d_gpuTime = myDiffTime(start, end);

	gettimeofday(&start, NULL);
	scanCPU(c, a, N);
	
	gettimeofday(&end, NULL);
	d_cpuTime = myDiffTime(start, end);
	

	cudaFree(dev_a); cudaFree(dev_g);

	for (int i = 0; i < N; i++)
	{
		printf("c[%i] = %0.3f, g[%i] = %0.3f\n", i, c[i], i, g[i]);
	}

	printf("GPU Time for scan size %i: %f\n", N, d_gpuTime);
	printf("CPU Time for scan size %i: %f\n", N, d_cpuTime);
	
	// Built bank functionality from scratch
	// Changed float to double for better accuracy
}

void bestscan(double *g_odata, double *g_idata, int n) {

	int size = n * sizeof(double);
	double* blocksum;
	int maxblocksize = B;
	int sharedmemory = maxblocksize + ((maxblocksize-1)>> LOG_NUM_BANKS);
	int num_thread_blocks = (n/B);
	// No shortage of blocks
	if((N % B) != 0)
		num_thread_blocks++;
	
	cudaMalloc(&blocksum, sizeof(double)*num_thread_blocks); //array of sums
	cudaMemset(blocksum, 0, sizeof(double)*num_thread_blocks);
	
	cudaMemset(g_odata, 0, size);
	
	prescan<<<num_thread_blocks,B/2,sharedmemory*sizeof(double)>>>(g_odata, g_idata, blocksum, n, sharedmemory, maxblocksize);
	
	if (num_thread_blocks <= maxblocksize)
	{
		//Only one block needed
		double* fillblocksum;
		cudaMalloc(&fillblocksum, sizeof(double));
		cudaMemset(fillblocksum, 0, sizeof(double));
		prescan<<<1, B/2, sharedmemory*sizeof(double)>>>(blocksum, blocksum, fillblocksum, num_thread_blocks, sharedmemory, maxblocksize);
		cudaFree(fillblocksum);
	}

	else
	{
		// Recursive function call for summation of multiple blocks
		double* blocksum2;
		cudaMalloc(&blocksum2, sizeof(double)*num_thread_blocks);
		cudaMemcpy(blocksum2, blocksum, sizeof(double)*num_thread_blocks, cudaMemcpyDeviceToDevice);
		bestscan(blocksum, blocksum2, num_thread_blocks);
		cudaFree(blocksum2);
	}
	
	gpusummation<<<num_thread_blocks, B/2>>>(g_odata, g_odata, blocksum, n);	
	
	cudaFree(blocksum);
}

__global__ void gpusummation(double *g_odata, double *g_idata, double* blocksum, int n) {
	
	// SUMMATION
	
	double sum = blocksum[blockIdx.x];
	int idx = 2*blockIdx.x*blockDim.x + threadIdx.x;
	
	if (n > idx)
	{
		g_odata[idx] = g_odata[idx] + sum;
		if (n > idx + blockDim.x)
			g_odata[idx + blockDim.x] = g_odata[idx+blockDim.x]+sum;
	}
}

//Heavily inspired by CUDA Gems3 book with slight changes to support bigger array sizes

__global__ void prescan(double *g_odata, double *g_idata, double *blocksum, int n, int sharedmemory, int maxblocksize) 
{ 
	extern  __shared__  double temp[];
	// allocated on invocation 
	int thid = threadIdx.x; 
	int offset = 1; 
	
	//A
	int ai = thid;
	int bi = thid + blockDim.x;
	int bankOffsetA = CONFLICT_FREE_OFFSET(ai);
	int bankOffsetB = CONFLICT_FREE_OFFSET(bi);
	int idx = maxblocksize * blockIdx.x + threadIdx.x;
	
	temp[thid] = 0;
	temp[thid+blockDim.x] = 0;

	if (thid + maxblocksize < sharedmemory)
		temp[thid + maxblocksize] = 0;

	__syncthreads();
	
	//Fill temporary shared buffer
	if (n > idx){
	
		temp[ai + bankOffsetA] = g_idata[idx];
		if (n > idx + blockDim.x)
			temp[bi + bankOffsetB] = g_idata[idx + blockDim.x];
	}
	
	for (int d = maxblocksize>>1; d > 0; d >>= 1) 
	// build sum in place up the tree 
    	{ 
        	__syncthreads(); 
		if (thid < d)    
        	{ 
			
			//B
				int ai = offset*(2*thid+1)-1;
				int bi = offset*(2*thid+2)-1;
				ai += CONFLICT_FREE_OFFSET(ai);
				bi += CONFLICT_FREE_OFFSET(bi);
		    	temp[bi] += temp[ai];         
				
			} 
        	offset *= 2; 
    	} 

		if (thid==0) { 
			blocksum[blockIdx.x] = temp[maxblocksize - 1 + CONFLICT_FREE_OFFSET(maxblocksize - 1)];
			temp[maxblocksize - 1 + CONFLICT_FREE_OFFSET(maxblocksize - 1)] = 0;
		}

	// clear the last element 
	for (int d = 1; d < maxblocksize; d *= 2) 
	// traverse down tree & build scan 
    	{ 
        	offset >>= 1; 
        	__syncthreads(); 
		if (thid < d) 
        	{ 
			
				//D
				int ai = offset*(2*thid+1)-1;
				int bi = offset*(2*thid+2)-1;
				ai += CONFLICT_FREE_OFFSET(ai);
				bi += CONFLICT_FREE_OFFSET(bi);
				
				
				double t   = temp[ai]; 
				temp[ai]  = temp[bi]; 
				temp[bi] += t; 
        	} 
    	} 
    	__syncthreads(); 
		
	//Output shared buffer
	if (n > idx)
	{
		g_odata[idx] = temp[ai + bankOffsetA];
		if (n > idx + blockDim.x)
			g_odata[idx + blockDim.x] = temp[bi + bankOffsetB];
	}
}
 
void scanCPU(double *f_out, double *f_in, int i_n)
{
	f_out[0] = 0;
	for (int i = 1; i < i_n; i++)
		f_out[i] = f_out[i-1] + f_in[i-1];

}
