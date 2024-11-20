// Will now add multi-threading capability

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <pthread.h>

#ifndef MAX_DIM
#define MAX_DIM 4000
#endif // MAX_DIM

#ifndef DEF_DUM
#define DEF_DIM 1000
#endif // MAX_DIM

#define MICROSECONDS_PER_SECOND 1000000.0

static float **matrix1 = NULL;
static float **matrix2 = NULL;
static float **result = NULL;
static int dim = DEF_DIM;
static int num_threads = 1;

float** alloc_matrix(void);					// Used to allocate the matrix
void free_matrix(float **);					// Used to free matrix in all dimensions
void init(float **, float **, float **); 	// Initializes all elements in matrix
void op_mat(float **);						// Outputs the result of the matrix	
void* mult(void*);							// Performs the matrix multiplication
double elapse_time(struct timeval *, struct timeval *);	// Calculates the elapsed time of a process 

int main(int argc, char *argv[]) {

	struct timeval et0;
	struct timeval et1;
	struct timeval et2;
	struct timeval et3;
	struct timeval et4;
	struct timeval et5;
	pthread_t *threads = NULL;
	long tid = 0;

	{
		int opt = -1;

		while ((opt = getopt(argc, argv, "t:d:h")) != -1) {
			switch(opt) {
				case 't':
					num_threads = atoi(optarg);
					break;
				case 'd':
					dim = atoi(optarg);
					if (dim < DEF_DIM) {
						dim = DEF_DIM;	
					}

					if (dim > MAX_DIM) {
						dim = MAX_DIM;
					}
					break;
				case 'h':
					printf("%s: -t # -d #\n", argv[0]);
					printf("\t-t #: number of threads\n");
					printf("\t-d #: size of matrix\n");
					exit(0);
					break;
				default: /* '?' */
					exit(EXIT_FAILURE);
					break;
			}
		}
	}

	gettimeofday(&et0, NULL);

	matrix1 = alloc_matrix();
	matrix2 = alloc_matrix();
	result = alloc_matrix();

	gettimeofday(&et1, NULL);

	init(matrix1, matrix2, result); // Initialize values of matrix
	threads = malloc(num_threads * sizeof(pthread_t)); // Initialize threads 

	gettimeofday(&et2, NULL);
	// mult();

	// Create a thread (up to num_threads) for the function mult
	for (tid = 0; tid < num_threads; tid++) {
		pthread_create(&threads[tid], NULL, mult, (void *) tid);
	}

	for (tid = 0; tid < num_threads; tid++) {
		pthread_join(threads[tid], NULL);	
	}

	gettimeofday(&et3, NULL);
	op_mat(result);

	gettimeofday(&et4, NULL);
	free_matrix(matrix1);
	free_matrix(matrix2);
	free_matrix(result);
	free(threads);

	matrix1 = matrix2 = result = NULL;

	gettimeofday(&et5, NULL);
	{
		double total_time = elapse_time(&et0, &et5);
		double alloc_time = elapse_time(&et0, &et1);
		double init_time = elapse_time(&et1, &et2);
		double comp_time = elapse_time(&et2, &et3);
		double op_time = elapse_time(&et3, &et4);
		double td_time = elapse_time(&et4, &et5);

		printf("Total time: %8.2lf\n", total_time);
		printf("  Alloc time: %8.2lf\n", alloc_time);
		printf("  Init  time: %8.2lf\n", init_time);
		printf("  Comp  time: %8.2lf\n", comp_time);
		printf("  O/P   time: %8.2lf\n", op_time);
		printf("  T/D   time: %8.2lf\n", td_time);
	}

	return EXIT_SUCCESS;



}


float** alloc_matrix(void) {

	// First allocate memory for float pointers
	float **mat = malloc(dim * sizeof(float *));

	int i = -1;

	// Dereference each float pointer and allocate memory for floats of size dimensions
	for (i = 0; i < dim; i++) {
		mat[i] = malloc(dim * sizeof(float));
	}

	return mat;

}


void free_matrix(float** mat) {

	int i = -1;

	// Free each float pointer
	for (i = 0; i < dim; i++) {
		free(mat[i]);
	}
	
	// Free pointer to float pointer
	free(mat);

}


void init(float** mat1, float** mat2, float** res) {

	int i = -1;
	int j = -1;

	// Initialize all values within the matrix
	// Will be using arbitrary, hard coded values instead of random number generator to save time
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			mat1[i][j] = (i + j) * 2.0;
			mat2[i][j] = (i + j) * 3.0;
			res[i][j] = 0.0;
		}
	}

}


void op_mat(float **) {

	FILE *op = NULL;
	int i = -1;
	int j = -1;

#define FILE_NAME "mm3.txt"
	op = fopen(FILE_NAME, "w");
	if (op == NULL) {
		perror("Could not open file: " FILE_NAME);
		exit(17);
	}

	// Iterate through matrix and display to file 
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			fprintf(op, "%8.2f ", result[i][j]); // Print floating point values with 2 decimal places, 8 width total 
		}
		fprintf(op, "\n");
	}

	fclose(op);

}

double elapse_time(struct timeval *t0, struct timeval *t1) {

	double et = (((double) (t1->tv_usec - t0->tv_usec)) // Get elapsed time of microseconds, end time - start time
					/ MICROSECONDS_PER_SECOND) 	// Convert microseconds to seconds by dividing by microseconds per second
			+ ((double) (t1->tv_sec - t0->tv_sec)); // Add the difference between end and start, in seconds

	return et;

}

void* mult(void* vid) {

	int i = -1;
	int j = -1;
	int k = -1;
	long tid = (long) (vid);

	for (i = tid; i < dim; i += num_threads) { // For each column in matrix 1
		for (j = 0; j < dim; j++) { // For each column in matrix 2
			for (k = 0; k < dim; k++) { // For each row in matrix 1 and 2
				// Result will catch the product of the element in matrix 1 and matrix 2
				result[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}
	pthread_exit(EXIT_SUCCESS);
}
