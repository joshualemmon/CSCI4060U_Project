#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

int* A;
int* B;
int** C;

void* generateMatrix(void* flatten_arg);
void* multiply(void* multiply_arg);

// struct initArgs
// {
// 	int i;
// 	int n;
// 	int m;
// };

struct gen_matrix_args
{
	int i;
	int n;
	int i_max;
	int m;
	int reverse;
};

struct multiply_args
{
	int n;
	int m;
	int k;
	int i_max;
	int i;
};

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
	int num_threads = atoi(argv[2]);
	if (num_threads < 1)
	{
		printf("Error: Expect num_threads > 0\n");
		exit(0);
	}
	pthread_t a_threads[num_threads];
	pthread_t b_threads[num_threads];
	pthread_t c_threads[num_threads];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	// Convert dimensions to ints
	int m = atoi(argv[3]);
	int k = atoi(argv[4]);
	int n = atoi(argv[5]);
	if (num_threads > m)
	{
		printf("Error: Too many threads for size of matrix A.\n");
		exit(0);
	}
	if (num_threads > k)
	{
		printf("Error: Too many threads for size of matrix B.\n");
		exit(0);
	}
	// Initialize memory space of C
	C = malloc(m*sizeof(int*));
	for (int i = 0; i < m; i++)
		C[i] = malloc(n*sizeof(int));
	// Initialize random seed
	srand(time(NULL));
	// Create array for generating matrix A
	// Find size of chunk to be given to each thread
	int chunk = m/num_threads;
	// Initialize A

	// Generate random flat matrix A of size m*k
	struct gen_matrix_args A_args[num_threads];
	// Initialize A
	A = malloc(m*k*sizeof(int));
	for (int i = 0; i < num_threads; i++)
	{
		A_args[i].i = i*chunk;
		A_args[i].i_max = i*chunk + chunk;
		A_args[i].n = m;
		A_args[i].m = k;
		A_args[i].reverse = 0;
		// Add remainder of iterations if there are leftovers
		if(A_args[i].i_max + chunk > m-1)
			A_args[i].i_max = m;

		int e = pthread_create(&a_threads[i], &attr, generateMatrix, (void*)&A_args[i]);

		if(e)
		{
			printf("Error creating matrix A flatten threads: %d\n", e);
			exit(-1);
		}
	}
	for (int i = 0; i < num_threads; i++)
	{
		int ea = pthread_join(a_threads[i], NULL);

		if(ea)
		{
			printf("Error joining matrix A flatten threads: %d\n", ea);
			exit(-1);
		}
	}
	chunk = k/num_threads;
	// Generate random flat matrix B of size k*n
	struct gen_matrix_args B_args[num_threads];
	// Initialize B
	B = malloc(k*n*sizeof(int));
	for (int i = 0; i < num_threads; i++)
	{
		B_args[i].i = i*chunk;
		B_args[i].i_max = i*chunk + chunk;
		B_args[i].n = k;
		B_args[i].m = n;
		B_args[i].reverse = 1;
		// Add remainder of iterations if there are leftovers
		if(B_args[i].i_max + chunk > k-1)
			B_args[i].i_max = k;

		int e = pthread_create(&b_threads[i], &attr, generateMatrix,(void*)&B_args[i]);

		if(e)
		{
			printf("Error creating matrix B flatten threads: %d\n", e);
			exit(-1);
		}
	}
	for (int i = 0; i < num_threads; i++)
	{
		int eb = pthread_join(b_threads[i], NULL);

		if(eb)
		{
			printf("Error joining matrix B flatten threads: %d\n", eb);
			exit(-1);
		}
	}
	if(output)
	{
		printf("Matrix A\n\n");
		printf("Flat:\n");
		for(int i = 0; i < n*k; i++)
			printf("%d ", A[i]);
		printf("\nUnflattened:\n");
		for(int i = 0; i < m; i++)
		{
			for (int j = 0; j < k; j++)
				printf("%d ", A[i*k + j]);
			printf("\n");
		}
		printf("--------\n");
		printf("Matrix B\n\n");
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
		printf("--------\n");
	}
	// Multiply A and B, putting results in global variable matrix C
	struct multiply_args m_args[num_threads];
	chunk = m/num_threads;
	for (int i = 0; i < num_threads; i++)
	{
		m_args[i].n = m;
		m_args[i].m = n;
		m_args[i].k = k;
		m_args[i].i_max = i*chunk + chunk;
		m_args[i].i = i*chunk;

		if(m_args[i].i_max + chunk > m-1)
			m_args[i].i_max = m;

		int e = pthread_create(&c_threads[i], &attr, multiply, (void*)&m_args[i]);
		if(e)
		{
			printf("Error creating multiplication threads: %d\n", e);
			exit(-1);
		}
	}

	for (int i = 0; i < num_threads; i++)
	{
		int e = pthread_join(c_threads[i], NULL);

		if(e)
		{
			printf("Error joining multiplication threads: %d\n", e);
			exit(-1);
		}
	}
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
void* generateMatrix(void* args)
{
	struct gen_matrix_args* arg;
	arg = (struct gen_matrix_args*) args;
	for (int i = arg->i; i < arg->i_max; i++)
		for (int j = 0; j < arg->m; j++)
			if (arg->reverse)
				B[j*arg->n + i] = rand() % 5;
			else
				A[i*arg->m + j] = rand() % 5; 
}

// Multiplies matrices A and B together
void* multiply(void* args)
{
	struct multiply_args* arg;
	arg = (struct multiply_args*) args;
	for (int i = arg->i; i < arg->i_max; i++)
		for (int j = 0; j < arg->m; j++)
		{	
			int total = 0;
			for (int l = 0; l < arg->k; l++)
				total += A[i*arg->k+l] * B[l+j*arg->k];
			C[i][j] = total;

		}
}