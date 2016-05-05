/* Fichier matrix.h */

#ifndef MATRIX_H
#define MATRIX_H


typedef float E;

typedef struct matrix {
	E *mat;
	int nrows, ncols;
	}*Matrix;


Matrix newMatrix(int nb_rows, int nb_columns);
E getElt(Matrix m, int row, int column);
void setElt(Matrix m, int row, int column, E val);
void deleteMatrix(Matrix m); 
void displayMatrix(Matrix);
Matrix newIdentite(int nb_rows);
Matrix aleatoire(int nb_rows, int nb_columns, int min, int max);

#endif
