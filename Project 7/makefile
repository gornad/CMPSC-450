all:
	gcc -g -o SerialMatrixMultiply SerialMatrixMultiply.c -lm -O3
	gcc -g -o OpenMPMatrixMultiply OpenMPMatrixMultiply.c -lm -fopenmp -O3
	mpicc -g -o SummaMPIMatrixMultiply SummaMPIMatrixMultiply.c -lm -O3