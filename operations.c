/* Fichier operations.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "operations.h"

Matrix addition(Matrix m, Matrix n){

	if(m->ncols != n->ncols && m->nrows != n->ncols){
		fprintf(stderr,"matrices de tailles différentes\n");
		return m;
	}
	int i,j;
	double s;
	Matrix M = newMatrix(m->nrows, m->ncols);
	for(i=0; i<m->nrows; i++){
		for(j=0; j<m->ncols; j++){
				s=getElt(m,i,j) + getElt(n,i,j);
				setElt(M, i, j, s);
		}
	}
	return M;
}

Matrix multiplication(Matrix m, Matrix n){
	int i,j,k;
	E s=0;
	Matrix M = newMatrix(m->nrows, m->ncols);
	for(i=0; i<m->nrows; i++){
		for(j=0; j<m->ncols; j++){
			setElt(M, i, j, 0);
			for(k=0; k<n->ncols; k++){
				s = getElt(M,i,j)+(getElt(m,i,k)*getElt(n,k,j));
				setElt(M, i, j, s);				
			}
		}
	}
	return M;
}

Matrix transpose(Matrix m){
	int i,j;
	Matrix M = newMatrix(m->ncols, m->nrows);
	for(i=0; i<m->nrows; i++){
		for(j=0; j<m->ncols; j++){
			setElt(M, j, i, getElt(m, i, j));
		}
	}
	return M;
}
