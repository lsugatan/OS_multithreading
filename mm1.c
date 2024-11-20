#include <stdio.h>
#include <stdlib.>
#include <uinstd.h>
#include <getopt.h>

#ifndef MAX_DIM
#define MAX_DIM 4000
#endif // MAX_DIM

#ifndef DEF_DUM
#define DEF_DIM 1000
#endif // MAX_DIM

static float **matrix1 = NULL;
static float **matrix2 = NULL;
static float **matrix3 = NULL;
static int dim = DEF_DIM;

float** alloc_matrix(void);					// Used to allocate the matrix
void free_matrix(float **);					// Used to free matrix in all dimensions
void init(float **, float **, float **); 	// Initializes all elements in matrix
void op_mat(float **);						// Outputs the result of the matrix	
void mult(void);

int main(int argc, char *argv[]) {

	{
		int opt = -1;

		while ((opt = getopt(argc, argv, "t:d:h")) != -1) {
			switch(opt) {
				case 't':
					break; // Ignoring this for now
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

	matrix1 = alloc_matrix();
	matrix2 = alloc_matrix();
	result = alloc_matrix();

	init(matrix1, matrix2, result);

	mult();
	op_mat(result);

	free_matrix(matrix1);
	free_matrix(matrix2);
	free_matrix(result);

	matrix1 = matrix2 = result = NULL;

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


void free_matrix(float **) {

	int i = -1;

	// Free each float pointer
	for (i = 0; i < dim; i++) {
		free(mat[i]);
	}
	
	// Free pointer to float pointer
	free(mat);

}


void init(float **, float **, float **) {

	int i = -1;
	int j = -1;

	// Initialize all values within the matrix
	// Will be using arbitrary, hard coded values instead of random number generator to save time
	for (int i = 0; i < dim; i++) {
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

#define FILE_NAME "mm1.txt"
	op = fopen(FILE_NAME, "w");
	if (op == NULL) {
		perror("Could not open file: " FILE_NAME);
		exit(17);
	}

	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			fprintf(op, "%8.2f ", result[i][j]); // Print 
		}
		fprintf(op, "\n");

	}

}
