// Fichier header de 


#ifndef _OPERATIONS_H__
#define _OPERATIONS_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"


// Fonctions dans le fichier source

// TP1
Matrix additionMatrix(Matrix m1, Matrix m2);

Matrix multiplicationMatrix(Matrix m1, Matrix m2);

Matrix transposeMatrix(Matrix m1);

// TP2
Matrix copyMatrix(const Matrix m);

Matrix multScal(E v, const Matrix m);

E normeVector(const Matrix v);

void swapLine(Matrix m, int l1, int l2);

void combineLines(Matrix m, E c1, int l1, E c2, int l2);

Matrix solve_gauss_simple(const Matrix A, const Matrix B);

Matrix solve_gauss_partiel(const Matrix A, const Matrix B);


#endif















