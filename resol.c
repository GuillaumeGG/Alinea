
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

Matrix multVector(Matrix M, Matrix v){
	if(v->ncols != 1){
		fprintf(stderr, "argument 2: matrice de taille nx1.\n");
		exit(0);
	}
	
	Matrix u = newMatrix(v->nrows, 1);
	int i, j;
	for(i=0; i<v->nrows; i++){
		setElt(u,i,0,0);
		for(j=0;j<v->nrows;j++){
			setElt(u,i,0,
					(getElt(u,i,0)+(getElt(M,i,j)*getElt(v,i,0))));
		}
	}
	return u;
}


E normeVector(const Matrix V){
	if(V->nrows > 1 && V->ncols > 1){
		fprintf(stderr, "argument: matrice de taille 1xn ou nx1.\n");
		return 0;
	}
	
	int i;
	E s=0;
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

Matrix puissance(Matrix A, float p){
	
	//On prend en argument un float dufait de la façon dont l'interpréteur
	//est codé, et on cast ce float en int
	int pu = (int)p;
	if(pu==0){
		return newIdentite(A->nrows);
	}
	Matrix cpy = copyMatrix(A);
	int i = 1;
	while(i<pu){
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
	free(temp);	
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
	free(temp);	
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
	deleteMatrix(C);
	deleteMatrix(D);
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
	deleteMatrix(A2);
	deleteMatrix(B);
	return det;
}

void addMultiple_Unique(Matrix A, int i, int j, E c){
	int k;
	for(k=0; k < A->nrows ; k++){
		
		setElt(A,i,k,
				(getElt(A,i,k) + c*getElt(A,j,k)));
	}
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
	
	//On évite de modifier la matrice d'entrée par effet de bord
	Matrix A = copyMatrix(A_to_invert);
	Matrix InvA = newIdentite(A->nrows);
	
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
			
		addMultiple_Inv(A,InvA,j,i, -1*(getElt(A,j,i)/getElt(A,i,i)));
		}
	}
	
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

	deleteMatrix(A);
	return InvA;
}



int rang(Matrix M){

	int i, j, rang=0, verif=0;
	Matrix A = copyMatrix(M);  

	for(i=0; i<A->nrows-1; i++){

		j = choixPivot(A,i);
		if(j != i){
			echangeLignes_Inv(A,i,j);
		}   

		for(j=i+1; j<A->nrows; j++){ 
				addMultiple_Unique(A,j,i, -1*(getElt(A,j,i)/getElt(A,i,i)));
		}
	}
	for(i=0; i<A->nrows; i++){    

		for(j=0; j<A->ncols; j++){
			if(getElt(A,i,j) == 0){
			verif++;
			}
		}

		if(verif != A->ncols){
			rang++;
		}
		verif=0;
	}
	deleteMatrix(A);
	return rang;

}



Matrix* decompositionLU(Matrix A_to_LU, Matrix* res){
	
	//On évite de modifier la matrice d'entrée par effet de bord
	Matrix A = copyMatrix(A_to_LU);

	Matrix U = newMatrix(A->nrows,A->nrows);
	Matrix L = newIdentite(A->nrows);
	//Matrix U = newIdentite(A->nrows);
	
	int i, j, k;
	E s=0;
	
	setElt(U, 0, 0, getElt(A, 0, 0));
	//Première ligne de U et LU
	for(j=1; j<A->nrows; j++){
		
		setElt(U, 0, j,
				getElt(A, 0, j));
		setElt(L, j, 0,
				(getElt(A, j, 0)/getElt(A, 0, 0)));
	}
	
	for(i = 1; i<A->nrows-1; i++){
		
		//Diagonale de la ligne i
		s=0;
		for(k = 0; k<i; k++){
			 s += getElt(L, i, k)*getElt(U, k, i);
		}
		setElt(U, i, i, getElt(A, i, i)-s);
		s=0;
		//Elements post-diagonale de la ligne i
		for(j=i+1; j<A->nrows; j++){
			
						
			for(k = 0; k<i; k++){
					s += getElt(L, i, k)*getElt(U, k, j);
			}
			setElt(U, i, j, getElt(A, i, j)-s);
			
			s=0;
			setElt(L, j, i, getElt(A, j, i));			
			for(k=0; k<i; k++){
				setElt(L, j, i,
					(getElt(L, j, i) - (getElt(L, j, k)*getElt(U, k, i))));
			}			
			setElt(L, j, i, (getElt(L, j, i)*(1/getElt(U,i,i))));
			
		}
	}
	//Element (n,n) de U
	setElt(U,A->nrows-1,A->nrows-1, getElt(A,A->nrows-1,A->nrows-1));
	
	for(k=0; k<A->nrows-1; k++){
		setElt(U, A->nrows-1, A->nrows-1,
				getElt(U, A->nrows-1, A->nrows-1)
					- (getElt(L, A->nrows-1, k)*getElt(U, k, A->nrows-1)));
	}
	
	//On place L et U dans le tableau de pointeurs sur matrices passé
	//en entrée, et on renvoie ce dernier.
	res[0] = L;
	res[1] = U;
	deleteMatrix(A);
	return res;
		
}

Matrix* valeurPropre(Matrix A, Matrix* res){
	
	int i=0;
	
	//On créé un vecteur composé uniquement de 1
	Matrix u = newMatrix(A->nrows,1);			
	for(i=0; i<u->nrows; i++)
		setElt(u,i,0,1);

	Matrix u2 = newMatrix(A->nrows,1);
	for(i=0; i<u->nrows; i++)
		setElt(u2,i,0,0);
		
	//Matrice utilisée pour éviter les memory leaks
	Matrix leak_preventer;
		
	//v est la composante de u de plus grande valeure absolue.
	//On représente v sous la forme d'une matrice 1x1 pour pouvoir
	//renvoyer un tableau de matrices contenant u et v.
	Matrix v = newMatrix(1,1);
	setElt(v,0,0,1);
	
	Matrix v2 = newMatrix(1,1);
	setElt(v2,0,0,0);
	
	E abs;	
	E approx = 0.0001;
	
	//Tant que la précision n'est pas suffisante:
	while( (getElt(v,0,0) - getElt(v2,0,0) > approx) || (normeVector(u)-normeVector(u2) > approx) ){

		leak_preventer = v2;
		v2 = copyMatrix(v); 
		free(leak_preventer);
		
		leak_preventer = u2;
		u2 = copyMatrix(u);
		free(leak_preventer);
		
		abs = 0;
		
		leak_preventer = u;
		u = multVector(A,u);
		free(leak_preventer);

		//v prend la valeur de la plus grande composante de u
		for(i=0; i<u->nrows; i++){
			if(fabs(getElt(u,i,0))>abs)
				abs = fabs(getElt(u,i,0));
		}					
		setElt(v,0,0, abs);
		
		leak_preventer = u;
		u = multScal(1/getElt(v,0,0), u);
		free(leak_preventer);
	}
	
	//On place u et v dans le tableau de pointeurs sur matrices passé
	//en entrée, et on renvoie ce dernier.
	res[0] = u;
	res[1] = v;
	deleteMatrix(u2);
	deleteMatrix(v2);
	return res;
}
