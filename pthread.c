#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

int* flat_A;
int* flat_B;
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
	int chunk;
	int m;
	int reverse;
	int tid;
};

struct multiply_args
{
	int n;
	int m;
	int k;
	int chunk;
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

	// Flatten matrix A into a 1d array of size m*k
	struct gen_matrix_args flat_a_args[num_threads];
	// Initialize flat_A
	flat_A = malloc(m*k*sizeof(int));
	for (int i = 0; i < num_threads; i++)
	{
		flat_a_args[i].i = i*chunk;
		flat_a_args[i].chunk = i*chunk + chunk;
		flat_a_args[i].n = m;
		flat_a_args[i].m = k;
		flat_a_args[i].reverse = 0;
		// Add remainder of iterations if there are leftovers
		if(flat_a_args[i].chunk + chunk > m-1)
			flat_a_args[i].chunk = m;

		int e = pthread_create(&a_threads[i], &attr, generateMatrix, (void*)&flat_a_args[i]);

		if(e)
		{
			printf("Error creating matrix A flatten threads: %d\n", e);
			exit(-1);
		}
	}
	// Flatten matrix B into 1d array of size k*n
	struct gen_matrix_args flat_b_args[num_threads];
	// Initialize flat_B
	flat_B = malloc(k*n*sizeof(int));
	for (int i = 0; i < num_threads; i++)
	{
		flat_b_args[i].i = i*chunk;
		flat_b_args[i].chunk = i*chunk + chunk;
		flat_b_args[i].n = k;
		flat_b_args[i].m = n;
		flat_b_args[i].reverse = 1;
		flat_b_args[i].tid = i;
		// Add remainder of iterations if there are leftovers
		if(flat_b_args[i].chunk + chunk > k-1)
			flat_b_args[i].chunk = k;

		int e = pthread_create(&b_threads[i], &attr, generateMatrix,(void*)&flat_b_args[i]);

		if(e)
		{
			printf("Error creating matrix B flatten threads: %d\n", e);
			exit(-1);
		}
	}
	for (int i = 0; i < num_threads; i++)
	{
		int ea = pthread_join(a_threads[i], NULL);
		int eb = pthread_join(b_threads[i], NULL);

		if(ea)
		{
			printf("Error joining matrix A flatten threads: %d\n", ea);
			exit(-1);
		}
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
			printf("%d ", flat_A[i]);
		printf("\nUnflattened:\n");
		for(int i = 0; i < m; i++)
		{
			for (int j = 0; j < k; j++)
				printf("%d ", flat_A[i*k + j]);
			printf("\n");
		}
		printf("--------\n");
		printf("Matrix B\n\n");
		printf("Flat:\n");
		for(int i = 0; i < n*k; i++)
			printf("%d ", flat_B[i]);
		printf("\nUnflattened:\n");

		for(int i = 0; i < k; i++)
		{
			for (int j = 0; j < n; j++)
				printf("%d ", flat_B[i + j*n]);
			printf("\n");
		}
		printf("--------\n");
	}
	// Multiply flattened A and flattened B, putting results in global variable matrix C
	struct multiply_args m_args[num_threads];
	chunk = m/num_threads;
	for (int i = 0; i < num_threads; i++)
	{
		m_args[i].n = m;
		m_args[i].m = n;
		m_args[i].k = k;
		m_args[i].chunk = i*chunk + chunk;
		m_args[i].i = i*chunk;

		if(m_args[i].chunk + chunk > m-1)
			m_args[i].chunk = m;

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

// Flattens a matrix M. If reverse is true then flattened
// array will be in column first format.
void* generateMatrix(void* args)
{
	struct gen_matrix_args* arg;
	arg = (struct gen_matrix_args*) args;
	for (int i = arg->i; i < arg->chunk; i++)
		for (int j = 0; j < arg->m; j++)
			if (arg->reverse)
				flat_B[j*arg->n + i] = rand() % 5;
			else
				flat_A[i*arg->m + j] = rand() % 5; 
}

// Multiplies matrices A and B together
void* multiply(void* args)
{
	struct multiply_args* arg;
	arg = (struct multiply_args*) args;
	for (int i = 0; i < arg->n; i++)
		for (int j = 0; j < arg->m; j++)
		{	
			int total = 0;
			for (int l = 0; l < arg->k; l++)
				total += flat_A[i*arg->k+l] * flat_B[l+j*arg->k];
			//pthread_mutex_lock(&mutex_C);
			C[i][j] = total;
			//pthread_mutex_unlock(&mutex_C);

		}
}