/* Fichier resol.h */

#ifndef RESOL_H
#define RESOL_H

#include "matrix.h"
#include "operations.h"

/*
 * renvoie la duplication d’une matrice entrée en paramètre
 */
Matrix copyMatrix(const Matrix m); 

/*
 * renvoie la matrice issue de la multiplication par un scalaire
 * de la matrice entrée en paramètre
 */
Matrix multScal(E v, const Matrix m);

/*
 * norme euclidienne d’une matrice stockant un vecteur (ligne ou colonne)
 */
E normeVector(const Matrix V);

/*
 * Renvoie la matrice A élevée à la puissance p
 */
Matrix puissance(Matrix A, int p);

/*
 * permute deux lignes d’une matrice
 */
void swapLine(Matrix m, int l1, int l2);

/*
 * combinant lineairement deux lignes d’une matrice en changeant la ligne 
 * l2 par : c1 * ligne l1 + c2 * ligne l2
 */
void combineLines(Matrix m, E c1, int l1, E c2, int l2);


Matrix remontee(Matrix A, Matrix B);


void addMultiple(Matrix A, Matrix B, int i, int j, E c);

int choixPivot(Matrix A, int i);

void echangeLignes(Matrix A, Matrix B, int i, int j);

void solveTriangulaire(Matrix A, Matrix B);

/*
 * renvoie le vecteur X solution du système A.X=B en effectuant un Pivot
 * de Gauss en choisissant le pivot le plus grand en valeur absolue
 */
Matrix solveGauss(const Matrix A, const Matrix B);

int determinant_opt_aux(Matrix A, Matrix B);

E determinant_opt(Matrix A);

void addMultiple_Inv(Matrix A, Matrix B,int i, int j, E c);

void echangeLignes_Inv(Matrix A, int i, int j);

Matrix invert(Matrix A);

void decompositionLU(Matrix A);

#endif

