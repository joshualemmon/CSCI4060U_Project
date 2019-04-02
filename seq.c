#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int** C;

int* generateMatrix(int n, int m, int reverse);
void multiply(int* A, int* B, int n, int m, int k);

int main(int argc, char* argv[])
{
	// Ensure all arguments are entered
	if (argc < 5)
	{
		printf("Error: Missing arguments.\n");
		exit(0);
	}
	// Flag for output prints
	int output = atoi(argv[1]);
	// Convert dimensions to ints
	int m = atoi(argv[2]);
	int k = atoi(argv[3]);
	int n = atoi(argv[4]);
	// Initialize memory space of C
	C = malloc(m*sizeof(int*));
	for (int i = 0; i < m; i++)
		C[i] = malloc(n*sizeof(int));
	// Initialize random seed
	srand(time(NULL));
	// Generate random flat matrix A of dimensions mxk
	int* A = generateMatrix(m, k, 0);
	if(output)
	{
		printf("Matrix A\n\n");
		for(int i = 0; i < n*k; i++)
			printf("%d ", A[i]);
		printf("\nUnflattened:\n");
		for(int i = 0; i < m; i++)
		{
			for (int j = 0; j < k; j++)
				printf("%d ", A[i*k + j]);
			printf("\n");
		}
		printf("\n--------\n");
	}
	// Generate random flat matrix B of dimensions kxn
	int* B = generateMatrix(k, n, 1);
	if (output)
	{
		printf("\nMatrix B\n\n");
		printf("Flat:\n");
		for(int i = 0; i < n*k; i++)
			printf("%d ", B[i]);
		printf("\nUnflattened:\n");
		for(int i = 0; i < k; i++)
		{
			for (int j = 0; j < n; j++)
				printf("%d ", B[i + j*n]);
			printf("\n");
		}
		printf("\n--------\n");
	}

	// Multiply flat A and flat B, putting results in global variable matrix C
	multiply(A, B, m, n, k);

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
int* generateMatrix(int n, int m, int reverse)
{
	int* flatM = malloc(n*m*sizeof(int));
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
		{
			if (reverse)
				flatM[j*n + i] = rand() % 5;
			else
				flatM[i*m + j] = rand() % 5; 
		}
	return flatM;
}

// Multiplies matrices A and B together
void multiply(int* A, int* B, int n, int m, int k)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			for (int l = 0; l < k; l++)
				C[i][j] += A[i*k+l] * B[l+j*k];
}