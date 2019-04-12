compile:
	gcc seq.c -o seq.out
	gcc omp.c -fopenmp -o omp.out
	gcc pthread.c -o pt.out -lpthread