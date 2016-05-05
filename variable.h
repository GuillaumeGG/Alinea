// Fichier des variables
// Auteur : Wasmer Audric et Grosshenny Guillaume

#ifndef VARIABLE_H
#define VARIABLE_H

#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include <ctype.h>
#include <string.h>
#include "operations.h"

// Definition de types utilisés //
typedef enum{VAR_NUMBER,
             VAR_ARRAY,
             VAR_STRING,
             VAR_MATRIX,
             VAR_SYMBOL,
             VAR_CALL,
             VAR_NULL,
              } var_type;

// Structure des variables //
typedef struct variable_s {
  int type;
  char element[]; 
} variable_t;

typedef struct tabAndFunc_s {
  int size;
  variable_t** element;
} variable_array_t;

typedef struct {
	float value;
} variable_number_t;

typedef struct {
	char* string;
} variable_symbol_t;

typedef struct environment_s {
  char* id_type;
  variable_t* value;
  struct environment_s *next;
} environment_t;

/* ---- Fonctions ---- */

// alloue la mémoire nécessaire
variable_t* varNew(int type);


// Copie un contenu dans une variable
variable_t* varCopy(variable_t* var);

// liberation de la mémoire
void varFree(variable_t* var);

void varPrint(variable_t*, int);

void freeEvt(environment_t*);

//
variable_t* varParser(char* input);

variable_t* eval(int argc, variable_t** argv, environment_t** evnt);

#endif
