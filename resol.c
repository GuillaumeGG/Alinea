
/* Fichier resol.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "resol.h"



Matrix copyMatrix(const Matrix m){
	int i,j;
	Matrix M = newMatrix(m->nrows, m->ncols);
	
	for(i=0; i<m->nrows; i++){
		for(j=0; j<m->ncols; j++){
			setElt(M, i, j, getElt(m, i, j));
		}
	}
	return M;
}



Matrix multScal(E v, const Matrix m){
	int i,j;
	E p;
	Matrix M = newMatrix(m->nrows, m->ncols);
	
	for(i=0; i<m->nrows; i++){
		for(j=0; j<m->ncols; j++){
			p = v * getElt(m, i, j);
			setElt(M, i, j, p);
		}
	}
	return M;
}



E normeVector(const Matrix V){
	if(V->nrows > 1 && V->ncols > 1){
		fprintf(stderr, "argument: matrice de taille 1xn ou nx1.\n");
		return 0;
	}
	
	int i;
	E s;
	if(V->nrows == 1){
		for(i=0; i<V->ncols; i++){
			s += getElt(V, 0, i)*getElt(V, 0, i);
		}
	}
	else if(V->ncols == 1){
		for(i=0; i<V->nrows; i++){
			s += getElt(V, i, 0)*getElt(V, i, 0);
		}
	}
	return sqrt(s);
}

Matrix puissance(Matrix A, int p){
	
	if(p==0){
		return newIdentite(A->nrows);
	}
	Matrix cpy = copyMatrix(A);
	int i = 1;
	while(i<p){
		cpy = multiplication(cpy,A);
		i++;
	}
	return cpy;
}



void swapLine(Matrix m, int l1, int l2){
	if(l1 < 0 || l1 > m->nrows || l2 < 0 || l2 > m->nrows)
		fprintf(stderr, "l1 ou l2 invalides\n");
	
	E *temp = malloc(m->ncols*sizeof(E));
	int i;
	
	for(i=0; i<m->ncols; i++){
		temp[i] = getElt(m, l1, i);
	}
	for(i=0; i<m->ncols; i++){
		setElt(m, l1, i, getElt(m, l2, i));
		setElt(m, l2, i, temp[i]);
	}
}



void combineLines(Matrix m, E c1, int l1, E c2, int l2){
	if(l1 < 0 || l1 > m->nrows || l2 < 0 || l2 > m->nrows)
		fprintf(stderr, "l1 ou l2 invalides\n");
	
	E *temp = malloc(m->ncols*sizeof(E));
	int i;
	
	for(i=0; i<m->ncols; i++){
		temp[i] = c1 * getElt(m, l1, i) + c2 * getElt(m, l2, i);
		setElt(m, l2, i, temp[i]);
	}	
}

Matrix remontee(Matrix A, Matrix B){
	int i,j;
	Matrix X = newMatrix(A->nrows, 1);
	for(i = A->nrows-1; i>=0 ; i--){
		
		setElt(X,i,0,
					getElt(B,i,0));
					
		for(j=i+1; j<A->nrows; j++){
			setElt(X,i,0, 
					(getElt(X,i,0) - (getElt(A,i,j)*getElt(X,j,0))));
		}
		setElt(X,i,0, 
					(getElt(X,i,0)/getElt(A,i,i)));
	}
	return X;
}

void addMultiple(Matrix A, Matrix B, int i, int j, E c){
	int k;
	for(k=0; k < A->nrows ; k++){
		
		setElt(A,i,k,
				(getElt(A,i,k) + c*getElt(A,j,k)));
	}
	setElt(B,i,0,
				(getElt(B,i,0) + c*getElt(B,j,0)));
}

int choixPivot(Matrix A, int i){
	int j,p,v;
	p = i;
	v = fabs(getElt(A,i,i));
	for(j=i+1; j<A->nrows; j++){
		if(fabs(getElt(A,j,i))>v){
			p = j;
			v = fabs(getElt(A,j,i));
		}
	}
	return p;
}
	
void echangeLignes(Matrix A, Matrix B, int i, int j){
	int k;
	E tmp;
	for(k=0; k<A->nrows; k++){
		tmp = getElt(A,i,k);
		setElt(A,i,k,
					getElt(A,j,k));
		setElt(A,j,k,
					tmp);
	}
	tmp = getElt(B,i,0);
	setElt(B,i,0,
				getElt(B,j,0));
				
	setElt(B,j,0,
				tmp);
}

void solveTriangulaire(Matrix A, Matrix B){
	int i, j;
	
	for(i=0; i<A->nrows-1; i++){
		
		j = choixPivot(A,i);
		echangeLignes(A,B,i,j);
		for(j=i+1; j<A->nrows; j++){
			
			addMultiple(A,B,j,i, (-getElt(A,j,i)/getElt(A,i,i)));
		}
	}
}

Matrix solveGauss(Matrix A, Matrix B){
	Matrix C = copyMatrix(A);
	Matrix D = copyMatrix(B);
	solveTriangulaire(C,D);
	Matrix X = remontee(C,D);
	return X;
}



int determinant_opt_aux(Matrix A, Matrix B){
	int i, j,c = 1;
	for(i=0; i<A->nrows; i++){

		j = choixPivot(A,i);
		if(j != i){
			c = c*(-1);
			echangeLignes(A,B,i,j);
		}

		for(j=i+1; j<A->nrows; j++){
			addMultiple(A,B,j,i, (-getElt(A,j,i)/getElt(A,i,i)));
		}
	}
	return c;
}

E determinant_opt(Matrix A){
	int i;
	E det = 1.0;
	Matrix A2 = copyMatrix(A);
	Matrix B = newMatrix(A->nrows, A->ncols);

	int c = determinant_opt_aux(A2, B);

	for(i=0; i<A->nrows; i++){
		det *= getElt(A2, i, i);
	}
	
	det = det * c;
	return det;
}


void addMultiple_Inv(Matrix A, Matrix B, int i, int j, E c){
	int k;
	for(k=0; k < A->nrows ; k++){
		
		setElt(A,i,k,
				(getElt(A,i,k) + c*getElt(A,j,k)));
		setElt(B,i,k,
				(getElt(B,i,k) + c*getElt(B,j,k)));
	}
}

void echangeLignes_Inv(Matrix A, int i, int j){
	int k;
	E tmp;
	for(k=0; k<A->nrows; k++){
		tmp = getElt(A,i,k);
		setElt(A,i,k,
					getElt(A,j,k));
		setElt(A,j,k,
					tmp);
	}
}

Matrix invert(Matrix A_to_invert){
	if(A_to_invert->nrows != A_to_invert->ncols){
		fprintf(stderr, "Une matrice non-carrée n'est pas inversible!\n");
		exit(1);
	}
	printf("Matrice d'entrée : \n");
	//On évite de modifier la matrice d'entrée par effet de bord
	Matrix A = copyMatrix(A_to_invert);
	displayMatrix(A);
	Matrix InvA = newIdentite(A->nrows);
	printf("Matrice identité : \n");
	displayMatrix(InvA);
	
	int i, j;
	E det = 1;
	
	//Triangularisation de la matrice A; On applique les mêmes opérations sur InvA
	for(i=0; i<A->nrows-1; i++){
		
		j = choixPivot(A,i);
		if(j != i){
			echangeLignes_Inv(A,i,j);
			echangeLignes_Inv(InvA,i,j);
			det = -1 * det;
		}
		
		for(j=i+1; j<A->nrows; j++){
			
		printf("%f\n",(-getElt(A,j,i)/getElt(A,i,i))); 
		addMultiple_Inv(A,InvA,j,i, -1*(getElt(A,j,i)/getElt(A,i,i)));
		}
	}
	
	printf("Matrice d'entrée après triangularisation: \n");
	displayMatrix(A);
	printf("\n");
	displayMatrix(InvA);	
	
	// Diagonalisation de A
    for(j=A->ncols-1; j>=0; j--){
		
        for(i=j-1; i>=0; i--){
            addMultiple_Inv(A, InvA, i, j, -1*(getElt(A, i, j)/getElt(A,j,j)));
        }
    }

	// Normalisation de la diagonale de InvA
	    for(i=0; i<InvA->nrows; i++){
		
        for(j=0; j<InvA->ncols; j++){
            setElt(InvA, i, j, (getElt(InvA, i, j)/getElt(A, i, i)));
        }
    }
	
	printf("Matrice d'entrée après inversion: \n");
	displayMatrix(A);
	printf("Matrice inverse : \n");
	displayMatrix(InvA);

	return InvA;
}

void decompositionLU(Matrix A_to_LU){
	
	//On évite de modifier la matrice d'entrée par effet de bord
	Matrix A = copyMatrix(A_to_LU);
	/*
	//Tableau de stockage des matrices élémentaires de A
	Matrix* tabE = malloc(2*A->nrows*sizeof(Matrix));
	int tabE_length = 0;
	
	Matrix Id = newIdentite(A->nrows);
	
	int i, j;
	//Triangularisation de la matrice A; On applique les mêmes opérations sur Id
	for(i=0; i<A->nrows; i++){

		
		j = choixPivot(A,i);
		echangeLignes_Inv(A,i,j);
		echangeLignes_Inv(Id,i,j);		
		//On ajoute la matrice élémentaire contenu dans Id dans tabE,
		//Puis on réinitialise Id.	
		
		printf("Echange de ligne %d et %d\n",i,j);
		printf("Matrice A : \n");
		displayMatrix(A);
		printf("\n");
		printf("Matrice Id : \n");
		displayMatrix(Id);
		printf("\n");
		tabE[tabE_length] = Id;
		tabE_length++;
		Matrix Id = newIdentite(A->nrows);
		
		
		
		
		for(j=i+1; j<A->nrows; j++){
			
			addMultiple_Inv(A,Id,j,i, (-getElt(A,j,i)/getElt(A,i,i)));
			
			printf("add %d et %d * %f\n",i,j,(-getElt(A,j,i)/getElt(A,i,i)));
			printf("Matrice A : \n");
			displayMatrix(A);
			printf("\n");
			printf("Matrice Id : \n");
			displayMatrix(Id);
			printf("\n");
			
			tabE[tabE_length] = Id;
			tabE_length++;
			Id = newIdentite(A->nrows);
		}
	}
	printf("\n");
	displayMatrix(A);
	printf("\n");
	//CALCUL DE U
	Matrix U = tabE[0];
	for(i=1; i<tabE_length; i++){
		U = multiplication(U,tabE[i]);
		displayMatrix(tabE[i]);
		printf("\n");
	}
	displayMatrix(U);
	printf("\n");
	U = multiplication(U,A);
	displayMatrix(A);
	printf("\n");
	displayMatrix(U);
	*/
	
	Matrix U = newMatrix(A->nrows,A->nrows);
	Matrix L = newIdentite(A->nrows);
	
	int i, j, k;
	
	setElt(U, 0, 0, getElt(A, 0, 0));
	
	for(j=1; j<A->nrows; j++){
		
		setElt(U, 0, j,
				getElt(A, 0, j));
		setElt(L, j, 0,
				(getElt(A, j, 0)/getElt(A, 0, 0)));
	}
	
	for(i = 1; i<A->nrows-1; i++){
		
		setElt(U, i, i, getElt(A, i, i));
		for(k = 0; k<i-1; k++){
			setElt(U, i, i,
					(getElt(U, i, i) - (getElt(L, i, k)*getElt(U, k, i))));
		}
		
		for(j=i+1; j<A->nrows; j++){
			
			setElt(U, i, j, getElt(A, i, j));
			
			for(k = 0; k<i-1; k++){
				setElt(U, i, j,
					(getElt(U, i, j) - (getElt(L, i, k)*getElt(U, k, j))));
			}
			
			setElt(L, j, i, getElt(A, j, i));
			
			for(k=0; k<i-1; k++){
				setElt(L, j, i,
					(getElt(L, j, i) - (getElt(L, j, k)*getElt(U, k, i))));
			}
			
			setElt(L, j, i, (getElt(L, j, i)*(1/getElt(U,i,i))));
			
		}
	}
	
	setElt(U,A->nrows-1,A->nrows-1, getElt(A,A->nrows-1,A->nrows-1));
	
	for(k=0; k<A->nrows-1; k++){
		setElt(U, A->nrows-1, A->nrows-1,
				getElt(U, A->nrows-1, A->nrows-1)
					- (getElt(L, A->nrows-1, k)*getElt(U, k, A->nrows-1)));
	}
	
	printf("Matrice L de A :\n");
	displayMatrix(L);
	printf("Matrice U de A :\n");
	displayMatrix(U);
	Matrix LU = multiplication(L,U);
	printf("Matrice LU de A: \n");
	displayMatrix(LU);
	
		
			
}
