// Fichier source du programme operations


#include "operations.h"
#include <math.h>


/** ---- TP1 ---- **/

Matrix additionMatrix(Matrix m1, Matrix m2)
{
  int i, j;
  E val;
  Matrix m = newMatrix(m1->nrows, m1->ncols);
  if((m1->nrows != m2->nrows) || (m1->ncols != m2->ncols))
  {
    fprintf(stderr, "Vos matrices n'ont pas les même dimensions. Une matrice vide a été renvoyée.\n");
    return m;
  }
  else
  {
    for(i=0; i<m1->nrows; i++)
    {
      for(j=0; j<m1->ncols; j++)
      {
        val = (getElt(m1, i, j)) + (getElt(m2, i, j));
        setElt(m, i, j, val);
      }
    }
    return m;
  }
}



// Fonction multiplicationMatrix qui multiplie deux matrices

Matrix multiplicationMatrix(Matrix m1, Matrix m2)
{
  int i, j, k;
  E val;
  Matrix m = newMatrix(m1->nrows, m1->ncols);
  if(m1->nrows != m2->ncols) 
  {
    fprintf(stderr, "Vos matrices n'ont pas les bonnes dimensions. Une matrice vide a été renvoyée.\n");
    return m;
  }
  else
  {
    for(i=0; i<m1->nrows; i++)
    {
      for(j=0; j<m2->ncols; j++)
      {
        for(k=0; k<m2->ncols; k++)
        {
          val += (getElt(m1, i, k)) * (getElt(m2, k, j));
        }
        setElt(m, i, j, val);
      }
    }
    return m;
  }
}


// Fonction transposeMatrix qui retourne la transposee d'une matrice

Matrix transposeMatrix(Matrix m1)
{
  int i, j;
  E val;
  Matrix m = newMatrix(m1->ncols, m1->nrows);
  for(i=0; i<m1->nrows; i++)
  {
    for(j=0; j<m1->ncols; j++)
    {
      val = getElt(m1, i, j);
      setElt(m, j, i, val);
    }
  }
  return m;
}



/** ---- TP2 ---- **/


Matrix copyMatrix(const Matrix m)
{
  int i,j;
  E val;
  Matrix mnew = newMatrix(m->ncols, m->nrows);
  for(i=0; i<m->nrows; i++)
  {
    for(j=0; j<m->ncols; j++)
    {
      val = getElt(m, i, j);
      setElt(mnew, i, j, val);
    }
  }
  return mnew;
}


Matrix multScal(E v, const Matrix m)
{
  int i,j;
  E val;
  Matrix mnew = newMatrix(m->ncols, m->nrows);
  for(i=0; i<m->nrows; i++)
  {
    for(j=0; j<m->ncols; j++)
    {
      v=0;
      val = (getElt(m, i, j)) * v;
      setElt(m, i, j, val);
    }
  }
  return mnew;
}

/*
E normeVector(const Matrix v)
{
  int i, jusqua=0, verif = 0;
  E val=0;
  if(v->nrows != 1 && v->ncols != 1)
  {
    fprintf(stderr, "ERROR_normeVector : La matrice rentrée n'est pas un vecteur, retour : 0\n");
    return 0;
  }
  else if(v->nrows == 1)
  {
    jusqua = v->ncols;
    verif = 1;
  }
  else
  {
    jusqua = v->nrows;
    verif = 2;
  }
  for(i=0; i<jusqua; i++)
  {
    if(verif == 1)
    {
      val += getElt(v, 0, i);
    }
    if(verif == 2)
    {
      val += getElt(v,i,0);
    }
  }
  return (E)sqrt((double)val);
} */


void swapLine(Matrix m, int l1, int l2)
{
  int i;
  E val;
  Matrix mcop = copyMatrix(m);
  for(i=0; i<m->ncols; i++)
  {
    val = getElt(mcop, l1, i);
    setElt(m, l2, i, val);
  }
  for(i=0; i<m->ncols; i++)
  {
    val = getElt(mcop, l2, i);
    setElt(m, l1, i, val);
  }
  deleteMatrix(mcop);
}


void combineLines(Matrix m, E c1, int l1, E c2, int l2)
{
  int i;
  E val;
  //Matrix mcop = copyMatrix(m);
  for(i=0; i<m->ncols; i++)
  {
    val = c1 * getElt(m, l1, i) + c2 * getElt(m, l2, i);
    setElt(m, l2, i, val);
  }
}


/*
Matrix Determinant_naif_matrix_inferieure(Matrix A, int lgn, int col)
{
  Matrix inferieure = newMatrix((A->nbrows - 1) , (A->nbcolumns - 1));
  int i=0, j=0;
  for(; (lgn+i)<A->nrows; i++)
  {
    if(col == 1) // Si la colone "barrée" est la première
    {
      for(j=0; (col+j)<A->ncols; j++)
      {
        setElt(inferieure, i, j, getElt(A, (lgn+i), (col+j)));
      }
    }
    else if(col == A->ncols) // Si la colone "barrée" est la dernière
    {
      for(j=0; j<(A->ncols-1); j++)
      {
        setElt(inferieure, i, j, getElt(A, (lgn+i), j));
      }
    }
    else // Si la colone "barrée" est entre la première et la dernière
    {
      for(j=0; j<col; j++)
      {
        setElt(inferieure, i, j, getElt(A, (lgn+i), j));
      }
      for(j=0; (col+j)<A->ncols; j++)
      {
        setElt(inferieure, i, j, getElt(A, (lgn+i), (col+j)));
      }
    }
  }
}


E determinant_naif(Matrix A)
{
  
}

*/


void addMultiple(Matrix A, Matrix B, int i, int j, E c)
{
  int k;
  for(k=0; k < A->nrows ; k++)
  {
    setElt(A,i,k,(getElt(A,i,k) + c*getElt(A,j,k)));
  }
  setElt(B,i,0,(getElt(B,i,0) + c*getElt(B,j,0)));
}

int choixPivot(Matrix A, int i)
{
  int j,p,v;
  p = i;
  v = fabs(getElt(A,i,i));
  for(j=i+1; j<A->nrows; j++)
  {
    if(fabs(getElt(A,j,i))>v)
    {
      p = j;
      v = fabs(getElt(A,j,i));
    }
  }
  return p;
}

    

void echangeLignes(Matrix A, Matrix B, int i, int j)
{
  int k;
  E tmp;
  for(k=0; k<A->nrows; k++)
  {
    tmp = getElt(A,i,k);
    setElt(A,i,k, getElt(A,j,k));
    setElt(A,j,k,tmp);
  }

  tmp = getElt(B,i,0);

  setElt(B,i,0, getElt(B,j,0));
  setElt(B,j,0,tmp);
}

void triangulaire(Matrix A, Matrix B)
{
  int i, j;
  for(i=0; i<A->nrows-1; i++)
  {
    j = choixPivot(A,i);
    echangeLignes(A,B,i,j);
    
    for(j=i+1; j<A->nrows; j++)
    {
      addMultiple(A,B,j,i, (-getElt(A,j,i)/getElt(A,i,i)));
    }
  }
}


void determinant_opt_aux(Matrix A, Matrix B, E *compt)
{
  int i, j;
  for(i=0; i<A->nrows-1; i++)
  {
    j = choixPivot(A,i);
    if(j != i)
    {
      *compt += 1;
      echangeLignes(A,B,i,j);
    }
    for(j=i+1; j<A->nrows; j++)
    {
      addMultiple(A,B,j,i, (-getElt(A,j,i)/getElt(A,i,i)));
    }
  }
}

/*
E determinant_opt(Matrix A)
{
  int i,j;
  E det = 0.0;
  Matrix B = newMatrix(A->nrows, A->ncols);
  E *compt = malloc(sizeof(E));
  determinant_opt_aux(A, B, compt);
  for(i=0; i<B->nrows; i++)
  {
    for(j=0; j<B->ncols; j++)
    {
      if(i==j)
      {
        det += getElt(B, i, j);
      }
    }
  }
  if((compt%2) != 0)
  {
    det *= -1;
  }
  return det;
}*/



































