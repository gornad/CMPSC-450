/* 
   Matrix Multiply

   Originally:
   From PVM: Parallel Virtual Machine
   A Users' Guide and Tutorial for Networked Parallel Computing

   Geist et al

   Reduced to a serial program for comparison.

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void get_walltime(double* wcTime) {

	struct timeval tp;

	gettimeofday(&tp, NULL);

	*wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);

}

void
InitBlock(double *a, double *b, double *c, int blk)
{
        int len, ind;
        int i,j;

        len = blk*blk;
        for (ind = 0; ind < len; ind++) 
                { a[ind] = (double)(rand()%1000)/100.0; c[ind] = 0.0; }
        for (i = 0; i < blk; i++) {
                for (j = 0; j < blk; j++)
                        b[j*blk+i] = (i==j)? 1.0 : 0.0;
        }
}

void
BlockMult(double* c, double* a, double* b, int blk) 
{
        int i,j,k;

        for (i = 0; i < blk; i++)
                for (j = 0; j < blk; j ++)
                        for (k = 0; k < blk; k++)
                                c[i*blk+j] += (a[i*blk+k] * b[k*blk+j]);
}

int
main(int argc, char* argv[])
{

        double *a, *b, *c;
		double d_S, d_E;
		
        /* Compute the array dim from the same parameters. */
        int sidesize;
        if (argc == 3)
                sidesize = atoi(argv[1]) * atoi(argv[2]);

        else {
                fprintf(stderr, "usage: mmult m blk\n");
                exit(1);
        }

        /* allocate the memory for the arrays. */
        a = (double*)malloc(sizeof(double)*sidesize*sidesize);
        b = (double*)malloc(sizeof(double)*sidesize*sidesize);
        c = (double*)malloc(sizeof(double)*sidesize*sidesize);
        /* check for valid pointers */
        if (!(a && b && c)) { 
                fprintf(stderr, "%s: out of memory!\n", argv[0]);
                free(a); free(b); free(c);
                exit(2);
        }

        /* initialize the arrays */
        InitBlock(a, b, c, sidesize);
		
		get_walltime(&d_S);
        /* Multiply. */
        BlockMult(c, a, b, sidesize);
		get_walltime(&d_E);

        /* check it */
        int i;
        for (i = 0 ; i < sidesize*sidesize; i++) 
                if (a[i] != c[i]) 
                        printf("Error a[%d] (%g) != c[%d] (%g) \n", i, a[i],i,c[i]);

        printf("Done.\n");
		printf("Total time = %f \n", d_E - d_S);
        free(a); free(b); free(c);
        return 0;
}


