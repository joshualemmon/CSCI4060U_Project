#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int** C;

int** generateMatrix(int n, int m);
int* flattenMatrix(int**M, int n, int m, int reverse);
void multiply(int* A, int* B, int n, int m, int k);

int main(int argc, char* argv[])
{
	if (argc < 5)
	{
		printf("Error: Missing arguments.\n");
		exit(0);
	}
	int n1 = atoi(argv[1]);
	int m1 = atoi(argv[2]);
	int n2 = atoi(argv[3]);
	int m2 = atoi(argv[4]);

	if(m1 != n2)
	{
		printf("Error: Expect m1 and n2 to be equal.\n");
		exit(0);
	}

	C = malloc(n1*sizeof(int*));
	for (int i = 0; i < n1; i++)
		C[i] = malloc(m2*sizeof(int));
	srand(time(NULL));
	int** A = generateMatrix(n1, m1);
	for(int i = 0; i < n1; i++)
	{
		for (int j = 0; j < m1; j++)
			printf("%d ", A[i][j]);
		printf("\n");
	}
	int* flatA = flattenMatrix(A, n1, m1, 0);
	printf("----\n");
	int** B = generateMatrix(n2, m2);
	for(int i = 0; i < n2; i++)
	{
		for (int j = 0; j < m2; j++)
			printf("%d ", B[i][j]);
		printf("\n");

	}
	int* flatB = flattenMatrix(B, n2, m2, 1);
	printf("----\n");

	multiply(flatA, flatB, n1, m2, n2);
	for(int i = 0; i < n1; i++)
	{
		for (int j = 0; j < m2; j++)
			printf("%d ", C[i][j]);
		printf("\n");
	}
}

int** generateMatrix(int n, int m)
{
	int** M;

	M = malloc(n*sizeof(int*));

	for (int i = 0; i < n; i++)
	{
		M[i] = malloc(m*sizeof(int));
		for (int j = 0; j < m; j++)
			M[i][j] = rand() % 5;
	}

	return M;
}

int* flattenMatrix(int** M, int n, int m, int reverse)
{
	int* flatM = malloc(n*m*sizeof(int));
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


void multiply(int* A, int* B, int n, int m, int k)
{
	int iOffset, jOffset;
	for (int i = 0; i < n; i++)
	{
		iOffset = i * k;
		for (int j = 0; j < m; j++)
		{
			jOffset = j*k;
			for (int l = 0; l < k; l++)
				C[i][j] += A[iOffset+l] * B[l+jOffset];
		}
	}
}