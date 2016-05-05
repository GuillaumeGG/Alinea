// Fichier source du programme matrix


#include "matrix.h"


// Corps du programme

/** ---- TP1 ---- **/

// Fonction newMatrix qui créé une matrice vide

Matrix newMatrix(int nbrows, int nbcolumns)
{
  int i, j, pos;
  int nbCases = nbrows * nbcolumns;
  Matrix matr= (Matrix)malloc(sizeof(struct matrix));
  matr->mat = (E*)malloc(nbCases*sizeof(E));
  matr->nrows = nbrows;
  matr->ncols = nbcolumns;
  for(i=0; i<nbrows; i++)
  {
    for(j=0;j<nbcolumns; j++)
    {
      pos = (i*nbcolumns) + j;
      matr->mat[pos] = 0.0;
    }
  }
  return matr;
}


// Fonction getElt qui permet de récupérer un élément d'une matrice

E getElt(Matrix m, int row, int column)
{
  if(row<0 || row>=m->nrows || column<0 || column>=m->ncols)
  {
    fprintf(stderr, "ERREUR : column ou row invalide : row : %d, column : %d.\n", row, column);
    return -1.0;
  }
  int pos = (row*m->ncols) + column;
  return m->mat[pos];
}


// Fonction setElt qui permet d'insérer un élément dans une matrice

void setElt(Matrix m, int row, int column, E val)
{
  if(row<0 || row>=m->nrows || column<0 || column>=m->ncols)
  {
    fprintf(stderr, "ERREUR : column ou row invalide : row : %d, column : %d.\n", row, column);
    return;
  }
  else
  {
    int pos = (row*m->ncols) + column;
    m->mat[pos] = val;
    return;
  }
}


// Fonction identite permettant de creer une matrice identite

Matrix identite(int nbrows, int nbcolumns)
{
  int i, j;
  Matrix m = newMatrix(nbrows, nbcolumns);
  if(nbrows==nbcolumns)
  {
    for(i=0; i<nbrows; i++)
    {
      for(j=0; j<nbcolumns; j++)
      {
        if(i==j)
        {
          setElt(m, i, j, 1.0);
        }
      }
    }
  }
  else
  {
    fprintf(stderr, "ERREUR : Vos dimensions ne sont pas bonnes pour une matrice identite. Une matrice vide a été renvoyée.");
  }
  return m;
}


// Fonction aleatoire qui renvoie une matrice remplie de nombre aleatoires compris entre min et max

Matrix aleatoire(int nbrows, int nbcolumns, int min, int max)
{
  int i, j;
  E val;
  Matrix m = newMatrix(nbrows, nbcolumns);
  for(i=0; i<nbrows; i++)
  {
    for(j=0; j<nbcolumns; j++)
    {
      val = (rand()/(E)RAND_MAX) * (max-min) + min;
      setElt(m, i, j, val);
    }
  }
  return m;
}


// Fonction deleteMatrix qui permet de libérer une matrice

void deleteMatrix(Matrix m)
{
  free(m->mat);
  free(m);
  return;
}


// Fonction displayMatrix qui permet d'afficher une matrice

void displayMatrix(Matrix m)
{
  int i, j;
  E val;
  printf("Matrice :\n");
  for(i=0; i<m->nrows; i++)
  {
    printf("| ");
    for(j=0; j<m->ncols;j++)
    {
      val = getElt(m, i, j);
      printf("%f ", val);
    }
    printf("|\n");
  }
  return;
}







