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
 * renvoie la matrice issue de la multiplication par un vecteur v
 * de la matrice M entrée en paramètre
 */
Matrix multVector(Matrix M, Matrix v);

/*
 * norme euclidienne d’une matrice stockant un vecteur (ligne ou colonne)
 */
E normeVector(const Matrix V);

/*
 * Renvoie la matrice A élevée à la puissance p
 */
Matrix puissance(Matrix A, float p);

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

/*
 * Fonction A(i,k) = A(i,k) + c*A(j,k) avec 0<=k<A->nrows.
 * Utilisée pour la résolution d'un système AX=B.
 */
void addMultiple(Matrix A, Matrix B, int i, int j, E c);

/*
 * Choix du pivot pour l'algorithme de Gauss
 */
int choixPivot(Matrix A, int i);

/*
 * Echange les lignes i et j dans les matrices A et B, dans le cadre
 * de la résolution d'un système AX=B par le pivot de Gauss
 */
void echangeLignes(Matrix A, Matrix B, int i, int j);

/*
 * Met le système AX=B sous forme triangulaire
 */
void solveTriangulaire(Matrix A, Matrix B);

/*
 * renvoie le vecteur X solution du système A.X=B en effectuant un Pivot
 * de Gauss en choisissant le pivot le plus grand en valeur absolue
 */
Matrix solveGauss(const Matrix A, const Matrix B);

/*
 * Fonction auxiliaire de la fonction determinant_opt
 */
int determinant_opt_aux(Matrix A, Matrix B);

/*
 * Renvoie le déterminant de la matrice A
 */
E determinant_opt(Matrix A);

/*
 * Fonction addMultiple appliquée à une unique matrice A
 */
void addMultiple_Unique(Matrix A, int i, int j, E c);

/*
 * Fonction addMultiple appliquée à deux matrices A et B
 */
void addMultiple_Inv(Matrix A, Matrix B,int i, int j, E c);

/*
 * Fonction echangeLignes appliquée à une unique matrice A
 */
void echangeLignes_Inv(Matrix A, int i, int j);

/*
 * Renvoie l'inverse de la matrice A
 */
Matrix invert(Matrix A);

/*
 * Renvoie le rang de la matrice A
 */
float rang(Matrix M);

/*
 * Retourne dans le tableau de pointeurs sur matrice res les matrices L
 * et U issues de la décomposition LU de la matrice A
 */
Matrix* decompositionLU(Matrix A, Matrix* res);

/*
 * Renvoie dans le tableau de pointeurs sur matrice res la plus grande 
 * valeur propre et le plus grand vecteur propre de la matrice A
 */
Matrix* valeurPropre(Matrix A, Matrix* res);

#endif

