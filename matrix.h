// Fichier header pour matrix.c


#ifndef _MATRIX_H__
#define _MATRIX_H__

// Includes et defines

#include <stdio.h>
#include <stdlib.h>


// Structures de données

typedef float E;

typedef struct matrix{
  E *mat;
  int nrows, ncols;
} *Matrix;


// Fonctions du fichier source


// TP1
Matrix newMatrix(int nbrows, int nbcolumns);

E getElt(Matrix m, int row, int column);

void setElt(Matrix m, int row, int column, E val);

Matrix identite(int nbrows, int nbcolumns);

Matrix aleatoire(int nbrows, int nbcolumns, int min, int max);

void deleteMatrix(Matrix m);

void displayMatrix(Matrix m);


#endif




















