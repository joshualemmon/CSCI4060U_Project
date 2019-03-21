#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int** C;

int** generateMatrix(int n, int m);
int* flattenMatrix(int**M, int n, int m, int reverse);
void multiply(int* A, int* B, int n, int m, int k);

int main(int argc, char* argv[])
{
	// Ensure all arguments are entered
	if (argc < 7)
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
	int n1 = atoi(argv[3]);
	int m1 = atoi(argv[4]);
	int n2 = atoi(argv[5]);
	int m2 = atoi(argv[6]);
	// Ensure m1 and n2 are equal
	if(m1 != n2)
	{
		printf("Error: Expect m1 and n2 to be equal.\n");
		exit(0);
	}
	// Initialize memory space of C
	C = malloc(n1*sizeof(int*));
	#pragma omp parallel for shared(C)
	for (int i = 0; i < n1; i++)
		#pragma omp atomic write
		C[i] = malloc(m2*sizeof(int));

	// Initialize random seed
	srand(time(NULL));
	// Generate random matrix A of dimension n1xm1
	int** A = generateMatrix(n1, m1);
	if(output)
	{
		printf("Matrix A\n\n");
		for(int i = 0; i < n1; i++)
		{
			for (int j = 0; j < m1; j++)
				printf("%d ", A[i][j]);
			printf("\n");
		}
		printf("\n--------\n");
	}
	// Flatten matrix A into 1d array of size n1*m1
	int* flatA = flattenMatrix(A, n1, m1, 0);
	// Generate random matrix B of dimensions n2xm2
	int** B = generateMatrix(n2, m2);
	if (output)
	{
		printf("\nMatrix B\n\n");
		for(int i = 0; i < n2; i++)
		{
			for (int j = 0; j < m2; j++)
				printf("%d ", B[i][j]);
			printf("\n");
		}
		printf("\n--------\n");
	}
	// Flatten matrix B into 1d array of size n2*m1
	int* flatB = flattenMatrix(B, n2, m2, 1);

	// Multiply flattened A and flattened B, putting results in global variable matrix C
	multiply(flatA, flatB, n1, m2, n2);
	if (output)
	{
		printf("\nMatrix C\n\n");
		for(int i = 0; i < n1; i++)
		{
			for (int j = 0; j < m2; j++)
				printf("%d ", C[i][j]);
			printf("\n");
		}
	}
}

// Generate a random matrix of dimensions nxm
int** generateMatrix(int n, int m)
{
	int** M;

	M = malloc(n*sizeof(int*));
	#pragma omp parallel for
	for (int i = 0; i < n; i++)
	{
		M[i] = malloc(m*sizeof(int));
		for (int j = 0; j < m; j++)
			M[i][j] = rand() % 5;
	}

	return M;
}

// Flattens a matrix M. If reverse is true then flattened
// array will be in column first format.
int* flattenMatrix(int** M, int n, int m, int reverse)
{
	int* flatM = malloc(n*m*sizeof(int));
	#pragma omp parallel for collapse(2)
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
		{
			if (reverse)
				flatM[j*n + i] = M[i][j];
			else
				flatM[i*m + j] = M[i][j]; 
		}

	return flatM;
}

// Multiplies matrices A and B together
void multiply(int* A, int* B, int n, int m, int k)
{
	#pragma omp parallel for shared(C)
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
		{
			int total = 0;
			for (int l = 0; l < k; l++)
				total += A[i*k+l] * B[l+j*k];
			#pragma omp atomic write
			C[i][j] = total;
		}
}