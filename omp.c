#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int** C;

int* generateMatrix(int n, int m, int reverse);
void multiply(int* A, int* B, int n, int m, int k);

int main(int argc, char* argv[])
{
	// Ensure all arguments are entered
	if (argc < 6)
	{
		printf("Error: Missing arguments.\n");
		exit(0);
	}
	// Flag for output prints
	int output = atoi(argv[1]);
	// Number of threads to use
	int num_threads = atoi(argv[2]);
	if (num_threads < 1)
	{
		printf("Error: Expect num_threads > 0\n");
		exit(0);
	}
	omp_set_num_threads(num_threads);

	// Convert dimensions to ints
	int m = atoi(argv[3]);
	int k = atoi(argv[4]);
	int n = atoi(argv[5]);
	// Initialize memory space of C
	C = malloc(m*sizeof(int*));
	#pragma omp parallel for shared(C)
	for (int i = 0; i < m; i++)
		#pragma omp atomic write
		C[i] = malloc(k*sizeof(int));

	// Initialize random seed
	srand(time(NULL));
	// Generate random flat matrix A of dimension mxn
	int* flatA = generateMatrix(m, k, 0);
	if(output)
	{
		printf("Matrix A\n\n");
		printf("Flat:\n");
		for(int i = 0; i < n*k; i++)
			printf("%d ", flatA[i]);
		printf("\nUnflattened:\n");
		for(int i = 0; i < m; i++)
		{
			for (int j = 0; j < k; j++)
				printf("%d ", flatA[i * k + j]);
			printf("\n");
		}
		printf("\n--------\n");
	}
	// Generate random matrix B of dimensions kxk
	int* flatB = generateMatrix(k, n, 1);
	if (output)
	{
		printf("\nMatrix B\n\n");
		printf("Flat:\n");
		for(int i = 0; i < n*k; i++)
			printf("%d ", flatB[i]);
		printf("\nUnflattened:\n");
		for(int i = 0; i < k; i++)
		{
			for (int j = 0; j < n; j++)
				printf("%d ", flatB[i +  j*n]);
			printf("\n");
		}
		printf("\n--------\n");
	}
	// Flatten matrix B into 1d array of size k*n

	// Multiply flattened A and flattened B, putting results in global variable matrix C
	multiply(flatA, flatB, m, k, n);
	if (output)
	{
		printf("\nMatrix C\n\n");
		for(int i = 0; i < m; i++)
		{
			for (int j = 0; j < n; j++)
				printf("%d ", C[i][j]);
			printf("\n");
		}
	}
}


// Generates a flat matrix M. If reverse is true then flattened
// array will be in column first format.
int* generateMatrix(int m, int n, int reverse)
{
	int* flatM = malloc(n*m*sizeof(int));
	#pragma omp parallel for collapse(2)
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
		{
			if (reverse)
				flatM[j*n + i] = rand() % 5;
			else
				flatM[i*m + j] = rand() % 5; 
		}

	return flatM;
}

// Multiplies matrices A and B together
void multiply(int* A, int* B, int m, int k, int n)
{
	#pragma omp parallel for shared(C)
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
		{
			int total = 0;
			for (int l = 0; l < k; l++)
				total += A[i*k+l] * B[l+j*k];
			C[i][j] = total;
		}
}