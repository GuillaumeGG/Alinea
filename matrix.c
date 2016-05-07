/* Fichier matrix.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"

Matrix newMatrix(int nb_rows, int nb_columns){
	Matrix M = malloc(sizeof(struct matrix));
	M->nrows = nb_rows;
	M->ncols = nb_columns;
	M->mat = malloc((M->nrows)*(M->ncols)*sizeof(E));

	int i=0,j=0;
	for(i=0; i < M->nrows; i++){
		for(j=0; j < M->ncols; j++){
			M->mat[M->ncols*i+j] = 0;
		}
	}
	return M;
}

E getElt(Matrix m, int row, int column){
	if(row < 0 || row >= m->nrows){
		fprintf(stderr,"valeur row incorrecte\n");
		exit(0);
	}
	 if(column < 0 || column >= m->ncols){
		fprintf(stderr,"valeur column incorrecte\n");
		exit(0);
	}
	int position = (row*m->ncols) + column;
	return (m->mat[position]);
}

void setElt(Matrix m, int row, int column, E val){
	if(row < 0 || row >= m->nrows || column < 0 || column >= m->ncols){
		fprintf(stderr,"valeur row/column incorrecte(s)\n");
		exit(0);
	}

	int position = (row*m->ncols) + column;
	m->mat[position] = val;
}

Matrix newIdentite(int nb_rows){
	Matrix M = malloc(sizeof(struct matrix));
	M->nrows = nb_rows;
	M->ncols = nb_rows;
	M->mat = malloc(nb_rows*nb_rows*sizeof(E));

	int i,j;
	for(i=0; i<M->nrows; i++){
		for(j=0; j<M->ncols; j++){
			if(i==j)
				setElt(M, i, j, 1);
			else
				setElt(M, i, j, 0);
		}
	}
	return M;
}

double frand_a_b(double a, double b){
    return (( rand()/(double)RAND_MAX ) * (b-a) + a);
}

Matrix aleatoire(int nb_rows, int nb_columns, int min, int max){
	Matrix M = malloc(sizeof(struct matrix));
	M->nrows = nb_rows;
	M->ncols = nb_columns;
	M->mat = malloc(nb_rows*nb_columns*sizeof(E));

	int i,j;
	for(i=0; i<M->nrows; i++){
		for(j=0; j<M->ncols; j++){
			setElt(M, i, j, frand_a_b(min, max));
		}
	}
	return M;
}	



void deleteMatrix(Matrix m){
	free(m->mat);
	free(m);
}

void displayMatrix(Matrix m){
	int i=0, j=0;
	for(i=0; i<m->nrows; i++){
		for(j=0; j<m->ncols; j++){
			printf("%f ",m->mat[m->ncols*i+j]);
		}
		printf("\n");
	}
}	


