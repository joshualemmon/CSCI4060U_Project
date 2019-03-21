#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

int** A;
pthread_mutex_t mutex_A;
int* flat_A;
pthread_mutex_t mutex_flat_A;
int** B;
pthread_mutex_t mutex_B;
int* flat_B;
pthread_mutex_t mutex_flat_B;
int** C;
pthread_mutex_t mutex_C;

//void* initializeC(void* initArg);
void* generateMatrix(void* gen_arg);
void* flattenMatrix(void* flatten_arg);
void* multiply(void* multiply_arg);

// struct initArgs
// {
// 	int i;
// 	int n;
// 	int m;
// };

struct gen_matrix_args
{
	char matrix;
	int i;
	int chunk;
	int n;
	int m;
	int tid;
};

struct flatten_matrix_args
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
};

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
	int num_threads = atoi(argv[2]);
	if (num_threads < 1)
	{
		printf("Error: Expect num_threads > 0\n");
		exit(0);
	}
	pthread_t threads[num_threads];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&mutex_A, NULL);
	pthread_mutex_init(&mutex_B, NULL);
	pthread_mutex_init(&mutex_C, NULL);
	pthread_mutex_init(&mutex_flat_A, NULL);
	pthread_mutex_init(&mutex_flat_B, NULL);
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
	for (int i = 0; i < n1; i++)
		C[i] = malloc(m2*sizeof(int));
	// Initialize random seed
	srand(time(NULL));
	// Create array for generating matrix A
	struct gen_matrix_args gen_a_args[num_threads];
	// Find size of chunk to be given to each thread
	int chunk = n1/num_threads;
	// Initialize A[]
	A = malloc(n1*sizeof(int*));
	// Generate random matrix A of dimensions n1xm1
	for (int i = 0; i < num_threads; i++)
	{
		gen_a_args[i].matrix = 'A';
		gen_a_args[i].i = i*chunk;
		gen_a_args[i].chunk = i*chunk + chunk;
		gen_a_args[i].n = n1;
		gen_a_args[i].m = m1;
		gen_a_args[i].tid = i;
		// Add remainder of iterations if there are leftovers
		if(gen_a_args[i].chunk + chunk > n1-1)
			gen_a_args[i].chunk = n1;
		
		int e = pthread_create(&threads[i], &attr, generateMatrix, (void*)&gen_a_args[i]);

		if(e)
		{
			printf("Error creating matrix A generation threads: %d\n", e);
			exit(-1);
		}
	}
	for (int i = 0; i < num_threads; i++)
	{
		int e = pthread_join(threads[i], NULL);
		if (e)
		{
			printf("Error joining matrix A generation threads: %d\n", e);
			exit(-1);
		}
	}
	if(output)
	{
		printf("Matrix A\n\n");
		for(int i = 0; i < n1; i++)
		{
			for (int j = 0; j < m1; j++)
				printf("%d ", A[i][j]);
			printf("\n");
		}
		printf("--------\n");
	}
	pthread_mutex_destroy(&mutex_A);
	// Flatten matrix A into a 1d array of size n1*m1
	struct flatten_matrix_args flat_a_args[num_threads];
	// Initialize flat_A
	flat_A = malloc(n1*m1*sizeof(int));
	for (int i = 0; i < num_threads; i++)
	{
		flat_a_args[i].i = i*chunk;
		flat_a_args[i].chunk = i*chunk + chunk;
		flat_a_args[i].n = n1;
		flat_a_args[i].m = m1;
		flat_a_args[i].reverse = 0;
		// Add remainder of iterations if there are leftovers
		if(flat_a_args[i].chunk + chunk > n1-1)
			flat_a_args[i].chunk = n1;

		int e = pthread_create(&threads[i], &attr, flattenMatrix, (void*)&flat_a_args[i]);

		if(e)
		{
			printf("Error creating matrix A flatten threads: %d\n", e);
			exit(-1);
		}
	}
	for (int i = 0; i < num_threads; i++)
	{
		int e = pthread_join(threads[i], NULL);

		if(e)
		{
			printf("Error joining matrix A flatten threads: %d\n", e);
			exit(-1);
		}
	}
	for (int i = 0; i < n1*m1; i++)
		printf("%d ", flat_A[i]);
	printf("\n");
	pthread_mutex_destroy(&mutex_flat_A);
	// Generate random matrix B of dimensions n2xm2
	struct gen_matrix_args gen_b_args[num_threads];
	// Find size of chunk to be given to each thread
	chunk = n2/num_threads;
	// Initialize B[]
	B = malloc(n2*sizeof(int*));
	for (int i = 0; i < num_threads; i++)
	{
		gen_b_args[i].matrix = 'B';
		gen_b_args[i].i = i*chunk;
		gen_b_args[i].chunk = i*chunk + chunk;
		gen_b_args[i].n = n2;
		gen_b_args[i].m = m2;
		gen_b_args[i].tid = i;
		// Add remainder of iterations if there are leftovers
		if(gen_b_args[i].chunk + chunk > n2-1)
			gen_b_args[i].chunk = n2;
		
		int e = pthread_create(&threads[i], &attr, generateMatrix, (void*)&gen_b_args[i]);

		if(e)
		{
			printf("Error creating matrix B generation threads: %d\n", e);
			exit(-1);
		}
	}
	for (int i = 0; i < num_threads; i++)
	{
		int e = pthread_join(threads[i], NULL);
		if (e)
		{
			printf("Error joining matrix B generation threads: %d\n", e);
			exit(-1);
		}
	}
	if(output)
	{
		printf("Matrix B\n\n");
		for(int i = 0; i < n2; i++)
		{
			for (int j = 0; j < m2; j++)
				printf("%d ", B[i][j]);
			printf("\n");
		}
		printf("--------\n");
	}
	pthread_mutex_destroy(&mutex_B);
	// Flatten matrix B into 1d array of size n2*m2
	struct flatten_matrix_args flat_b_args[num_threads];
	// Initialize flat_B
	flat_B = malloc(n2*m2*sizeof(int));
	for (int i = 0; i < num_threads; i++)
	{
		flat_b_args[i].i = i*chunk;
		flat_b_args[i].chunk = i*chunk + chunk;
		flat_b_args[i].n = n2;
		flat_b_args[i].m = m2;
		flat_b_args[i].reverse = 1;
		flat_b_args[i].tid = i;
		// Add remainder of iterations if there are leftovers
		if(flat_b_args[i].chunk + chunk > n2-1)
			flat_b_args[i].chunk = n2;

		int e = pthread_create(&threads[i], &attr, flattenMatrix,(void*)&flat_b_args[i]);

		if(e)
		{
			printf("Error creating matrix B flatten threads: %d\n", e);
			exit(-1);
		}
	}
	for (int i = 0; i < num_threads; i++)
	{
		int e = pthread_join(threads[i], NULL);

		if(e)
		{
			printf("Error joining matrix B flatten threads: %d\n", e);
			exit(-1);
		}
	}
	for (int i = 0; i < n2*m2; i++)
		printf("%d ", flat_B[i]);
	printf("\n");
	pthread_mutex_destroy(&mutex_flat_B);
	// Multiply flattened A and flattened B, putting results in global variable matrix C
	multiply((void*)1);

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
void* generateMatrix(void* args)
{
	struct gen_matrix_args* arg;
	arg = (struct gen_matrix_args*) args;
	for (int i = arg->i; i < arg->chunk; i++)
	{
		if(arg->matrix == 'A')
		{
			pthread_mutex_lock(&mutex_A);
			A[i] = malloc(arg->m*sizeof(int));
			pthread_mutex_unlock(&mutex_A);
		}
		else if(arg->matrix == 'B')
		{
			pthread_mutex_lock(&mutex_B);
			B[i] = malloc(arg->m*sizeof(int));
			pthread_mutex_unlock(&mutex_B);
		}

		for (int j = 0; j < arg->m; j++)
		{
			if(arg->matrix == 'A')
			{
				pthread_mutex_lock(&mutex_A);
				A[i][j] = rand() % 5;
				pthread_mutex_unlock(&mutex_A);
			}
			if(arg->matrix == 'B')
			{
				pthread_mutex_lock(&mutex_B);
				B[i][j] = rand() % 5;
				pthread_mutex_unlock(&mutex_B);
			}
		}
	}
}

// Flattens a matrix M. If reverse is true then flattened
// array will be in column first format.
void* flattenMatrix(void* args)
{
	struct flatten_matrix_args* arg;
	arg = (struct flatten_matrix_args*) args;
	for (int i = arg->i; i < arg->chunk; i++)
		for (int j = 0; j < arg->m; j++)
		{
			if (arg->reverse)
			{
				pthread_mutex_lock(&mutex_flat_B);
				flat_B[j*arg->n + i] = B[i][j];
				pthread_mutex_unlock(&mutex_flat_B);
			}
			else
			{
				pthread_mutex_lock(&mutex_flat_A);
				flat_A[i*arg->m + j] = A[i][j]; 
				pthread_mutex_unlock(&mutex_flat_A);
			}
		}
}

// Multiplies matrices A and B together
void* multiply(void* args)
{
	int n = 0;
	int m = 0;
	int k = 0;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			for (int l = 0; l < k; l++)
				C[i][j] += flat_A[i*k+l] * flat_B[l+j*k];
}