// Fichier des variables
// Auteur : Wasmer Audric et Grosshenny Guillaume

#include <stdio.h>
#include <sys/time.h>

#include "resol.h"
#include "variable.h"

int bleh(char* str) {
  return fprintf(stderr, "Bleh. %s\n", str);
}

// Fonctions

/* VAR_NUMBER,
            VAR_ARRAY,
            VAR_STRING,
            VAR_MATRIX,
            VAR_SYMBOL,
            VAR_CALL,
            VAR_NULL, */

//void varPrint(variable_t* var, int indent);

static int size_elem(int type)
{
  int size;
  switch(type)
  {
    case VAR_NUMBER : size = sizeof(float);
                      break;
    case VAR_STRING : size = sizeof(char*);
                      break;
    case VAR_MATRIX : size = sizeof(struct matrix);
                      break;
    case VAR_SYMBOL : size = sizeof(char*);
                      break;
    case VAR_ARRAY : 
    case VAR_CALL  :  size = sizeof(variable_array_t);
                      break;
  case VAR_FUNCTION:
    size = sizeof(variable_function_t);
    break;
    case VAR_NULL :  size = 0;
                      break;
    default :
    size = -1; /* error */
    fprintf(stderr, "[varNew] : unknown type\n");

  }
  return size;
}


variable_t* varNew(int type)
{
  int size = size_elem(type);
  variable_t* var;
  
  var = malloc(sizeof(struct variable_s) + size);
  var->type = type;
  if(type == VAR_ARRAY || type == VAR_CALL)
  {
    variable_array_t* element = (variable_array_t*)var->element;
    element->element = NULL;
    element->size = 0;
  }
  if(type == VAR_MATRIX)
  {
    struct matrix** varMatrix;
    varMatrix = (struct matrix**)var->element;
    *varMatrix = NULL;
  }
  return var;
}

variable_t* varNewFunction(variable_t* (*f)(int, variable_t**))
{
  variable_function_t* vf;
  variable_t* rValue = varNew(VAR_FUNCTION);

  vf = (variable_function_t*) rValue->element;
  vf->function = f;

  return rValue;
}

variable_t* varNewMatrix(struct matrix* matrix)
{
  variable_t* r;
  struct matrix** m;

  r = varNew(VAR_MATRIX);

  m = (struct matrix**) r->element;
  *m = matrix;

  return r;
}

variable_t* varNewSymbol(char* str)
{
  variable_t* rValue = varNew(VAR_SYMBOL);

  memcpy(rValue->element, &str, sizeof(char*));

  return rValue;
}

variable_t* varNewNumber(float n)
{
  variable_t* rValue = varNew(VAR_NUMBER);

  memcpy(rValue->element, &n, sizeof(float));

  return rValue;
}

float varGetNumber(variable_t* var)
{
  return *(float*) var->element;
}

void varFree(variable_t* var)
{
  int i=0;
  variable_array_t* varTemp;
  switch(var->type)
  {
    case VAR_NULL :
    case VAR_NUMBER :       // libération en fin de fonction suffisante
                      break;
    case VAR_STRING : 
    case VAR_SYMBOL : free(*((void**)var->element));
                      break;
    case VAR_MATRIX : deleteMatrix(*((struct matrix**)var->element));
                      break;
    case VAR_ARRAY : 
    case VAR_CALL  :  varTemp = (variable_array_t*)var->element;
                      for(i=0;i<varTemp->size;i++)
                      {
                        varFree(varTemp->element[i]);
                      }
                      free(varTemp->element);
                      break;
    default : break;
  }
  free(var);
}


variable_t* varCopy(variable_t* var)
{
  int i=0;
  variable_t *varCp;
  variable_array_t *varTemp, *varCpTemp;
  variable_t **array;

  switch(var->type)
  {
    case VAR_NUMBER :
      varCp = varNew(var->type);
      memcpy(varCp->element, var->element, sizeof(float));
      break;

    case VAR_STRING : 
    case VAR_SYMBOL :
      varCp = varNewSymbol(strdup(*(char**) var->element));
      break;
    case VAR_ARRAY :
    case VAR_CALL :
      varCp = varNew(var->type);
      varTemp = (variable_array_t*)var->element;
      varCpTemp = (variable_array_t*)varCp->element;
      varCpTemp->size = varTemp->size;
      array = malloc(sizeof(*array) * varTemp->size);

      for(i=0;i<varTemp->size;i++)
      {
        array[i] = varCopy(varTemp->element[i]);
      }

      varCpTemp->element = array;
      break;

    case VAR_MATRIX :
      {
        variable_t* copy;
        struct matrix** nMatrix;

        copy = varNew(VAR_MATRIX);
        nMatrix = (struct matrix**) copy->element;
        *nMatrix = copyMatrix(*(struct matrix**) var->element);

        varCp = copy;

        break;
      }
  case VAR_FUNCTION:
    {
      variable_function_t* orig = (variable_function_t*) var->element;
      varCp = varNewFunction(orig->function);
      break;
    }
    case VAR_NULL :
      varCp = varNew(VAR_NULL);
      break;
  }

  return varCp;
}

int isSymbol(char c)
{
  return isalpha(c) || isdigit(c) || c == '_' || c == ':' || c == '+' || c == '-' || c == '*' || c == '/';
}

int offsetSymbol(char* input, int offsetCur)
{
  int i=offsetCur;
  for(;isSymbol(input[i]);i++)
  ;;
  return i;
}

int offsetNumber(char* input, int offsetCur)
{
  int i=offsetCur;
  for(;(isdigit(input[i])); i++)
  ;;

  if(input[i] == '.')
  {
    i++;
    for(; isdigit(input[i]); i++)
      ;;
  }
  return i;
}

static void matrixPrettyLine(int width, int top)
{
  int i;

  if (top)
  {
    printf("╭ ");

    for (i = 0; i < width; i++)
      printf("      ");

    printf("╮\n");
  }
  else
  {
    printf("╰ ");

    for (i = 0; i < width; i++)
      printf("      ");

    printf("╯\n");
  }
}

static void indentLine(int indent)
{
  int n;

  for (n = 0; n < indent; n++)
  {
    printf("┃ ");
  }
}

static void matrixPrettyPrint(struct matrix* m, int indent)
{
  char buffer[6];
  int i, j;
  float value;

  indentLine(indent);
  matrixPrettyLine(m->ncols, 1);

  for (i = 0; i < m->nrows; i++)
  {
    indentLine(indent);

    printf("│ ");

    for (j = 0; j < m->ncols; j++)
    {
      int n;

      value = getElt(m, i, j);

      snprintf(buffer, sizeof(buffer), "%.4f", value);

      for (n = 0; ((unsigned) n) < strlen(buffer) - 5; n++)
        printf(" ");

      if (value == 0.)
        printf("\033[01;30m%s\033[00m ", buffer);
      else
        printf("\033[37m%s\033[00m ", buffer);
    }

    printf("│\n");
  }

  indentLine(indent);
  matrixPrettyLine(m->ncols, 0);

  return;
}

void varArrayPush(variable_array_t* array, variable_t* var)
{
  array->element = realloc(array->element, (array->size+1)*sizeof(variable_t*)); // Allouer une place de variable supplémentaire
  array->element[array->size] = var;
  array->size++;
}

void varPrint(variable_t* var, int indent)
{
  int i;
  variable_array_t* entete;

  switch(var->type)
  {
    case VAR_NULL :
      indentLine(indent);
      printf("<␀>\n");
      break;
    case VAR_ARRAY :
    case VAR_CALL : 
      entete = (variable_array_t*)var->element;

      indentLine(indent);
      printf("┎\n");

      for(i=0;i<entete->size;i++)
      {
        varPrint(entete->element[i], indent + 1);
      }

      indentLine(indent);
      printf("┖\n");
      break;
    case VAR_MATRIX :
      matrixPrettyPrint(*(struct matrix**) var->element, indent);
      break;
    case VAR_NUMBER :
      indentLine(indent);
      printf("<𝒩  %f>\n", *(float*) var->element);
      break;
    case VAR_STRING:
    case VAR_SYMBOL:
      indentLine(indent);
      printf("<𝒮  %s>\n", *((char**)var->element));
      break;
  case VAR_FUNCTION:
    indentLine(indent);
    {
      variable_function_t* vf = (variable_function_t*) var->element;
      printf("<ℱ %p>\n", vf->function);
      break;
    }
  }
}

void varParserAux(char* input, char close, int* offset, variable_t* var)
{
  int debug=0, sizeTemp=0;
  
  // On prend la var tabFunc de var
  variable_array_t* array = (variable_array_t*)var->element;
  variable_t* varTemp;
  
  for(;input[*offset] != '\0' && input[*offset] != close; (*offset)++)
  {
  if (isSymbol(input[*offset]) && !isdigit(input[*offset]) && input[*offset] != '-')
    {
      char* bufferTemp;
      varTemp = varNew(VAR_SYMBOL);
      
      debug = offsetSymbol(input, *offset);
      
      sizeTemp = debug - *offset + 1;
      bufferTemp = malloc(sizeTemp);
      
      
      snprintf(bufferTemp, sizeTemp, "%s", (input)+*offset);
      
      memcpy(varTemp->element, &bufferTemp, sizeof(char*)); // Magie noire pour récupérer le symbol """"""""""simplement""""""""

      varArrayPush(array, varTemp);
      
      *offset = debug-1;
    }
    else if(isdigit(input[*offset]) || input[*offset]=='-')
    {
      float number;
      varTemp = varNew(VAR_NUMBER);
      debug = offsetNumber(input, *offset + 1);

      if (debug - 1 == *offset)
    {
    if (input[*offset] == '-')
    {
      varArrayPush(array, varNewSymbol(strdup("-")));

      *offset = debug - 1;

      continue;
    }
    }
      
      number = atof(input + *offset);
      
      memcpy(varTemp->element, &number, sizeof(float));
      
      varArrayPush(array, varTemp);
      
      *offset = debug-1;
    }
    else if((input[*offset] == '(') || (input[*offset] == '[') )
    {
      char newClose;
      debug = *offset;
      if(input[*offset] == '(')
      {
        newClose= ')';
        varTemp = varNew(VAR_CALL);
      }
      else
      {
        newClose = ']';
        varTemp = varNew(VAR_ARRAY);
      }
      *offset = (*offset) + 1;
      varParserAux(input, newClose, offset, varTemp);
      varArrayPush(array, varTemp);
    }
    
    if(input[*offset] == '\0')
    {
      break;
    }
  }

  return ;
}


variable_t* varParser(char* input)
{
  int offset = 0;
  variable_t* var = varNew(VAR_ARRAY);
  varParserAux(input, '\0', &offset, var);
  return var;
}


environment_t* newEvt(variable_t* value, char* id)
{
  environment_t* evt = malloc(sizeof(struct environment_s));
  evt->id_type = id;
  evt->value = value;
  evt->next = NULL;
  
  return evt;
}

void setEvt(environment_t** e, variable_t* value, char* key)
{
  environment_t* t;

  key = strdup(key);

  t = *e;

  if (!t)
  {
    *e = newEvt(value, key);

    return;
  }

  while (t)
  {
    if (!strcmp(t->id_type, key))
    {
      varFree(t->value);
      t->value = value;

      return;
    }

    t = t->next;
  }

  t = newEvt(value, key);
  t->next = *e;
  *e = t;
}

variable_t* getEvt(environment_t* e, char* key)
{
  while (e)
  {
    if (!strcmp(e->id_type, key))
      return e->value;

    e = e ->next;
  }

  return NULL;
}

void freeEvt(environment_t *e)
{
  environment_t* t;

  for (; e; e = t)
  {
    t = e->next;

    free(e->id_type);
    varFree(e->value);
    free(e);
  }
}

variable_t* f_matrix(int argc, variable_t** argv)
{
  int i, width = 0;
  variable_t* argv1;
  variable_array_t* array = NULL;
  variable_t* output;
  struct matrix* outputMatrix;

  if (argc != 2)
  {
    return varNew(VAR_NULL);
  }

  if ((argv1 = argv[1])->type != VAR_ARRAY)
  {
    return varNew(VAR_NULL);
  }

  array = (variable_array_t*) argv1->element;

  for (i = 0; i < array->size; i++)
  {
    variable_array_t* line = (variable_array_t*) array->element[i]->element;

    if (array->element[i]->type != VAR_ARRAY)
    {
      bleh("Not an array of numbers.");

      return varNew(VAR_NULL);
    }

    if (i == 0)
      width = line->size;
    else if (width != line->size)
    {
      bleh("Not the same size.");

      return varNew(VAR_NULL);
    }
  }

  output = varNew(VAR_MATRIX);
  outputMatrix = newMatrix(array->size, width);
  memcpy(output->element, &outputMatrix, sizeof(outputMatrix));

  for (i = 0; i < array->size; i++)
  {
    int j;
    variable_array_t* line = (variable_array_t*) array->element[i]->element;

    for (j = 0; j < width; j++)
    {
      variable_t* cell = (variable_t*) line->element[j];
      variable_number_t* value = (variable_number_t*) cell->element;

      /* Insert value here. */
      setElt(outputMatrix, i, j, value->value);
    }
  }

  return output;
}

variable_t* f_add(int argc, variable_t** argv)
{
  variable_array_t* array_mat;
  variable_t *mat1, *mat2;
  variable_t *output;
  struct matrix *mat1_value, *mat2_value, *result_add;
  
  if (argc != 2)
  {
    bleh("Incorrect arg number in f_add\n");
    return varNew(VAR_NULL);
  }
  
  array_mat = (variable_array_t*)argv[1]->element;
  
  mat1 = array_mat->element[0];
  mat2 = array_mat->element[1];
  
  if(mat1->type != VAR_MATRIX && mat2->type != VAR_MATRIX)
  {
    bleh("Incorrect variable type in f_add\n");
    printf("type mat1 : %d et type mat2 : %d\n",mat1->type, mat2->type);
    return varNew(VAR_NULL);
  }
  
  mat1_value = *(struct matrix**) mat1->element;
  mat2_value = *(struct matrix**) mat2->element;
  
  result_add = addition(mat1_value, mat2_value);
  
  output = varNew(VAR_MATRIX);
  memcpy(output->element, &result_add, sizeof(result_add));

  return output;
}

variable_t* f_plus(int argc, variable_t** argv)
{
  if (argc != 3)
  {
    bleh("+ a b");
    return varNew(VAR_NULL);
  }

  if (argv[1]->type == VAR_NUMBER && argv[2]->type == VAR_NUMBER)
  {
    return varNewNumber((*(float*) argv[1]->element) + (*(float*) argv[2]->element));
  }
  else if (argv[1]->type == VAR_MATRIX && argv[2]->type == VAR_MATRIX)
  {
    return varNewMatrix(addition(*(struct matrix**) argv[1]->element, *(struct matrix**) argv[2]->element));
  }

  return varNew(VAR_NULL);
}

variable_t* f_minus(int argc, variable_t** argv)
{
  if (argc != 3)
  {
    bleh("- a b");
    return varNew(VAR_NULL);
  }

  if (argv[1]->type == VAR_NUMBER && argv[2]->type == VAR_NUMBER)
  {
    return varNewNumber((*(float*) argv[1]->element) - (*(float*) argv[2]->element));
  }
  else if (argv[1]->type == VAR_MATRIX && argv[2]->type == VAR_MATRIX)
  {
    return varNewSymbol(strdup("not implemented"));
    //return varNewMatrix(addition(*(struct matrix**) argv[1]->element, *(struct matrix**) argv[2]->element));
  }

  return varNew(VAR_NULL);
}

variable_t* f_mult(int argc, variable_t** argv)
{
  variable_array_t* array_mat;
  variable_t *mat1, *mat2;
  variable_t *output;
  struct matrix *mat1_value, *mat2_value, *result_mult;
  
  if (argc != 2)
  {
    bleh("Incorrect arg number in f_mult\n");
    return varNew(VAR_NULL);
  }
  
  array_mat = (variable_array_t*)argv[1]->element;
  
  mat1 = array_mat->element[0];
  mat2 = array_mat->element[1];
  
  if(mat1->type != VAR_MATRIX && mat2->type != VAR_MATRIX)
  {
    bleh("Incorrect variable type in f_mult\n");
    printf("type mat1 : %d et type mat2 : %d\n",mat1->type, mat2->type);
    return varNew(VAR_NULL);
  }
  
  mat1_value = *(struct matrix**) mat1->element;
  mat2_value = *(struct matrix**) mat2->element;
  
  result_mult = multiplication(mat1_value, mat2_value);
  
  output = varNew(VAR_MATRIX);
  memcpy(output->element, &result_mult, sizeof(result_mult));

  return output;
}


variable_t* f_multScal(int argc, variable_t** argv)
{
  variable_array_t* array_operation;
  variable_t *mat, *scalar;
  variable_t *output;
  struct matrix *mat_value, *result_multScal;
  float scalar_value;
  
  if (argc != 2)
  {
    bleh("Incorrect arg number in f_multScal\n");
    return varNew(VAR_NULL);
  }
  
  array_operation = (variable_array_t*)argv[1]->element;
  
  scalar = array_operation->element[0];
  mat = array_operation->element[1];
  
  if(mat->type != VAR_MATRIX && scalar->type != VAR_NUMBER)
  {
    if(scalar->type != VAR_MATRIX && mat->type != VAR_NUMBER)
    {
      bleh("Incorrect variable type in f_multScal\n");
      printf("type mat : %d et type scalar : %d\n",mat->type, scalar->type);
      return varNew(VAR_NULL);
    }
    if(scalar->type == VAR_MATRIX && mat->type == VAR_NUMBER)
    {
      scalar = array_operation->element[1];
      mat = array_operation->element[0];
    }
  }
  
  scalar_value = *(float*) scalar->element;
  printf("Valeur de scalar : scalar_value : %f\n", scalar_value);
  mat_value = *(struct matrix**) mat->element;
  
  result_multScal = multScal(scalar_value, mat_value);
  
  output = varNew(VAR_MATRIX);
  memcpy(output->element, &result_multScal, sizeof(result_multScal));

  return output;
}

variable_t* f_pow(int argc, variable_t** argv)
{
  variable_array_t* array_operation;
  variable_t *mat, *pow;
  variable_t *output;
  struct matrix *mat_value, *result_pow;
  float pow_value;
  
  if (argc != 2)
  {
    bleh("Incorrect arg number in f_pow\n");
    return varNew(VAR_NULL);
  }
  
  array_operation = (variable_array_t*)argv[1]->element;
  
  mat = array_operation->element[0];
  pow = array_operation->element[1];
  
  if(mat->type != VAR_MATRIX && pow->type != VAR_NUMBER)
  {
    bleh("Incorrect variable type in f_pow\n");
    printf("type mat : %d et type pow : %d\n",mat->type, pow->type);
    return varNew(VAR_NULL);
  }
  
  pow_value = *(float*) pow->element;
  mat_value = *(struct matrix**) mat->element;
  
  result_pow = puissance(mat_value, pow_value);
  
  output = varNew(VAR_MATRIX);
  memcpy(output->element, &result_pow, sizeof(result_pow));

  return output;
}

variable_t* f_trans(int argc, variable_t** argv)
{
  variable_array_t* array_operation;
  variable_t *mat;
  variable_t *output;
  struct matrix *mat_value, *result_trans;
  
  if (argc != 2)
  {
    bleh("Incorrect arg number in f_trans\n");
    return varNew(VAR_NULL);
  }
  
  array_operation = (variable_array_t*)argv[1]->element;
  
  mat = array_operation->element[0];
  
  if(mat->type != VAR_MATRIX)
  {
    bleh("Incorrect variable type in f_trans\n");
    printf("type mat : %d \n",mat->type);
    return varNew(VAR_NULL);
  }
  
  mat_value = *(struct matrix**) mat->element;
  
  result_trans = transpose(mat_value);
  
  output = varNew(VAR_MATRIX);
  memcpy(output->element, &result_trans, sizeof(result_trans));

  return output;
}

variable_t* f_det(int argc, variable_t** argv)
{
  variable_array_t* array_operation;
  variable_t *mat;
  variable_t *output;
  struct matrix *mat_value;
  float result_det;
  
  if (argc != 2)
  {
    bleh("Incorrect arg number in f_trans\n");
    return varNew(VAR_NULL);
  }
  
  array_operation = (variable_array_t*)argv[1]->element;
  
  mat = array_operation->element[0];
  
  if(mat->type != VAR_MATRIX)
  {
    bleh("Incorrect variable type in f_trans\n");
    printf("type mat : %d \n",mat->type);
    return varNew(VAR_NULL);
  }
  
  mat_value = *(struct matrix**) mat->element;
  
  result_det = determinant_opt(mat_value);
  
  output = varNew(VAR_NUMBER);
  memcpy(output->element, &result_det, sizeof(result_det));

  return output;
}

variable_t* f_invert(int argc, variable_t** argv)
{
  variable_array_t* array_operation;
  variable_t *mat;
  variable_t *output;
  struct matrix *mat_value, *result_invert;
  
  if (argc != 2)
  {
    bleh("Incorrect arg number in f_trans\n");
    return varNew(VAR_NULL);
  }
  
  array_operation = (variable_array_t*)argv[1]->element;
  
  mat = array_operation->element[0];
  
  if(mat->type != VAR_MATRIX)
  {
    bleh("Incorrect variable type in f_trans\n");
    printf("type mat : %d \n",mat->type);
    return varNew(VAR_NULL);
  }
  
  mat_value = *(struct matrix**) mat->element;
  
  result_invert = invert(mat_value);
  
  output = varNew(VAR_MATRIX);
  memcpy(output->element, &result_invert, sizeof(result_invert));

  return output;
}

variable_t* f_rank(int argc, variable_t** argv)
{
  variable_array_t* array_operation;
  variable_t *mat;
  variable_t *output;
  struct matrix *mat_value;
  float result_rank;
  
  if (argc != 2)
  {
    bleh("Incorrect arg number in f_rank\n");
    return varNew(VAR_NULL);
  }
  
  array_operation = (variable_array_t*)argv[1]->element;
  
  mat = array_operation->element[0];
  
  if(mat->type != VAR_MATRIX)
  {
    bleh("Incorrect variable type in f_rank\n");
    printf("type mat : %d \n",mat->type);
    return varNew(VAR_NULL);
  }
  
  mat_value = *(struct matrix**) mat->element;
  
  result_rank = rang(mat_value);
  
  output = varNewNumber(result_rank);

  return output;
}

variable_t* f_solve(int argc, variable_t** argv)
{
  variable_array_t* array_mat;
  variable_t *mat1, *mat2;
  variable_t *output;
  struct matrix *mat1_value, *mat2_value, *result_solve;
  
  if (argc != 2)
  {
    bleh("Incorrect arg number in f_solve\n");
    return varNew(VAR_NULL);
  }
  
  array_mat = (variable_array_t*)argv[1]->element;
  
  mat1 = array_mat->element[0];
  mat2 = array_mat->element[1];
  
  if(mat1->type != VAR_MATRIX && mat2->type != VAR_MATRIX)
  {
    bleh("Incorrect variable type in f_solve\n");
    printf("type mat1 : %d et type mat2 : %d\n",mat1->type, mat2->type);
    return varNew(VAR_NULL);
  }
 

  mat1_value = *(struct matrix**) mat1->element;
  mat2_value = *(struct matrix**) mat2->element;
  
  result_solve = solveGauss(mat1_value, mat2_value);
  
  output = varNew(VAR_MATRIX);
  memcpy(output->element, &result_solve, sizeof(result_solve));

  return output;
}

variable_t* f_valP(int argc, variable_t** argv)
{
  variable_array_t *array_operation, *array_result_table;
  variable_t *mat;
  variable_t *output_matrix, *output_val, *array_result_var;
  struct matrix *mat_value, **result_valP;
  float val_valP;
  
  if (argc != 2)
  {
    bleh("Incorrect arg number in f_valP\n");
    return varNew(VAR_NULL);
  }
  
  array_operation = (variable_array_t*)argv[1]->element;
  
  mat = array_operation->element[0];
  
  if(mat->type != VAR_MATRIX)
  {
    bleh("Incorrect variable type in f_valP\n");
    printf("type mat : %d \n",mat->type);
    return varNew(VAR_NULL);
  }
  
  mat_value = *(struct matrix**) mat->element;
  
  result_valP = malloc(3*sizeof(struct matrix*));
  result_valP = valeurPropre(mat_value, result_valP);
  
  output_matrix = varNewMatrix(result_valP[0]);
  
  val_valP = getElt(result_valP[1], 0, 0);
  output_val = varNewNumber(val_valP);

  array_result_var = varNew(VAR_ARRAY);
  
  array_result_table = (variable_array_t*) array_result_var->element;
  
  array_result_table->size = 2;
  array_result_table->element = malloc(sizeof(variable_t*) * 2);
  
  array_result_table->element[0] = output_matrix;
  array_result_table->element[1] = output_val;
  
  deleteMatrix(result_valP[1]);
  free(result_valP);
  
  return array_result_var;
}

variable_t* f_LU(int argc, variable_t** argv)
{
  variable_array_t *array_operation, *array_result_table;
  variable_t *mat;
  variable_t *output_matrix1, *output_matrix2, *array_result_var;
  struct matrix *mat_value, **result_LU;
  
  if (argc != 2)
  {
    bleh("Incorrect arg number in f_LU\n");
    return varNew(VAR_NULL);
  }
  
  array_operation = (variable_array_t*)argv[1]->element;
  
  mat = array_operation->element[0];
  
  if(mat->type != VAR_MATRIX)
  {
    bleh("Incorrect variable type in f_LU\n");
    printf("type mat : %d \n",mat->type);
    return varNew(VAR_NULL);
  }
  
  mat_value = *(struct matrix**) mat->element;
  
  result_LU = malloc(3*sizeof(struct matrix*));
  result_LU = decompositionLU(mat_value, result_LU);
  
  output_matrix1 = varNewMatrix(result_LU[0]);
  
  output_matrix2 = varNewMatrix(result_LU[1]);

  array_result_var = varNew(VAR_ARRAY);
  array_result_table = (variable_array_t*) array_result_var->element;
  array_result_table->size = 2;
  array_result_table->element = malloc(sizeof(variable_t*) * 2);
  array_result_table->element[0] = output_matrix1;
  array_result_table->element[1] = output_matrix2;
  
  free(result_LU);
  return array_result_var;
}

variable_t* f_echo(int argc, variable_t** argv)
{
  int i;

  for (i = 1; i < argc; i++)
  {
    varPrint(argv[i], 0);
  }

  return varNew(VAR_NULL);
}

variable_t* f_get(int argc, variable_t** argv)
{
  variable_t* arg;
  float i, j;

  if (argc != 4)
  {
    bleh("Wrong parameters: get matrix i j");

    return varNew(VAR_NULL);
  }

  arg = argv[2];
  if (arg->type != VAR_NUMBER)
  {
    bleh("Wrong parameters: get matrix i j");

    return varNew(VAR_NULL);
  }
  else
    memcpy(&i, arg->element, sizeof(float));

  arg = argv[3];
  if (arg->type != VAR_NUMBER)
  {
    bleh("Wrong parameters: get matrix i j");

    return varNew(VAR_NULL);
  }
  else
    memcpy(&j, arg->element, sizeof(float));

  arg = argv[1];
  if (arg->type != VAR_MATRIX)
  {
    bleh("Wrong parameters: get matrix i j");

    return varNew(VAR_NULL);
  }

  return varNewNumber(getElt(*(struct matrix**) arg->element, (int) i, (int) j));
}

variable_t* f_matrix_random(int argc, variable_t** argv)
{
  int i;
  int width = 3, height = 3;
  float min = 0;
  float max = 1;

  for (i = 1; i < 5; i++)
  {
    if (i < argc)
    {
      float n;
      variable_t* arg = argv[i];

      if (arg->type == VAR_SYMBOL && !strcmp(*(char**) arg->element, "_"))
        continue;

      if (arg->type != VAR_NUMBER)
      {
        varPrint(arg, 0);
        bleh("Argument is not a number!");

        return varNew(VAR_NULL);
      }

      memcpy(&n, arg->element, sizeof(float));

      switch (i) {
        case 1:
          width = (int) n;
          break;
        case 2:
          height = (int) n;
          break;
        case 3:
          min = n;
          break;
        case 4:
          max = n;
          break;
      }
    }
  }

  if (min > max)
  {
    float t = max;

    max = min;
    min = t;
  }

  return varNewMatrix(aleatoire(height, width, min, max));
}

/**
 * Black magic function of doom.
 *
 * Tests the speed of things.
 */
variable_t* f_speedtest(int argc, variable_t** argv)
{
	variable_t* arg;
	variable_array_t* argArray;
	int min, max, step;
	int i;
	struct timeval totalStop, totalStart;
	struct timeval stop, start;
	FILE* csv;
	long int startTime = -1;

	if (argc != 5)
	{
		bleh("speedtest command minSize maxSize step");

		return varNew(VAR_NULL);
	}

	if (argv[1]->type != VAR_FUNCTION)
	{
		bleh("argv[1] is no function");

		return varNew(VAR_NULL);
	}

	min = (int) varGetNumber(argv[2]);
	max = (int) varGetNumber(argv[3]);
	step = (int) varGetNumber(argv[4]);

	printf("  %i -> %i\n", min, max);

	csv = fopen("/tmp/speedtest.csv", "w");

	arg = varNew(VAR_ARRAY);
	argArray = (variable_array_t*) arg->element;

	argArray->size = 2;
	argArray->element = malloc(sizeof(variable_t*) * 2);
	argArray->element[0] = varCopy(argv[1]);

	gettimeofday(&totalStart, NULL);

	for (i = max; i >= min; i -= step)
	{
		//  char buffer[256];
		variable_t* pair;
		variable_array_t* array;

		pair = varNew(VAR_ARRAY);
		array = (variable_array_t*) pair->element;
		array->size = 2;
		array->element = malloc(sizeof(variable_t*) * 2);

		array->element[0] = varNewMatrix(aleatoire(i, i, 0, 1));
		array->element[1] = varNewMatrix(aleatoire(i, i, 0, 1));

		argArray->element[1] = pair;

		gettimeofday(&start, NULL);

		eval(2, argArray->element, NULL);

		gettimeofday(&stop, NULL);

		long int time = stop.tv_usec - start.tv_usec;
		if (startTime == -1)
			startTime = time;

		int color = (i / step) % 2 == 0 ? 4 : 6;

		printf("\033[00;4%im", color);
		int j = 0;
		for (j; j < (69 * time) / startTime; j++)
			printf(" ");
		printf("\033[00;3%im %li\n", color, time);

		if (time > 0)
			fprintf(csv, "%i, %li\n", i, time);

		varFree(pair);
	}

	gettimeofday(&totalStop, NULL);

	fclose(csv);

	system("echo '"
			"set datafile separator \",\"\n"
			"plot \"/tmp/speedtest.csv\" using 1:2 with lines\n"
		"' | gnuplot -persistant");

	varFree(argArray->element[0]);
	free(argArray->element);
	free(arg);

	return varNewNumber((float) (totalStop.tv_usec - totalStart.tv_usec));
}

variable_t* eval(int argc, variable_t** argv, environment_t** evnt)
{
  int i;
  variable_t* rValue = NULL;

  if (argc == 1)
  {
    if (argv[0]->type == VAR_SYMBOL)
    {
      variable_symbol_t* symbol = (variable_symbol_t*) argv[0]->element;
      variable_t* match = getEvt(*evnt, symbol->string);

      if (match)
        return varCopy(match);
      else
      {
        return varCopy(argv[0]);
      }
    }
    else if (argv[0]->type == VAR_ARRAY)
    {
      variable_array_t* orig;
      variable_array_t* array;

      rValue = varNew(VAR_ARRAY);
      orig = (variable_array_t*) argv[0]->element;
      array = (variable_array_t*) rValue->element;

      array->size = orig->size;
      array->element = malloc(sizeof(*array->element) * array->size);

      for (i = 0; i < orig->size; i++)
      {
        array->element[i] = eval(1, &orig->element[i], evnt);
      }

      return rValue;
    }
    else if (argv[0]->type == VAR_CALL)
    {
      variable_array_t* orig;

      orig = (variable_array_t*) argv[0]->element;

      rValue = eval(orig->size, orig->element, evnt);

      return rValue;
    }
    else
      return varCopy(argv[0]);
  }
  else if (argc >= 3)
  {
    if (argv[0]->type == VAR_SYMBOL && argv[1]->type == VAR_SYMBOL)
    {
      if (!strcmp(*((char**) argv[1]->element), ":"))
      {
        variable_t* rValue;
        char* varName;

        varName = *(char**)(argv[0]->element);
        rValue = eval(argc - 2, argv + 2, evnt);

        setEvt(evnt, rValue, varName);
        printf("%p\n", *evnt);

        printf("New variable: %s\n", varName);

        return varCopy(rValue);
      }
    }
  }

  variable_t** realValues;

  realValues = malloc(sizeof(variable_t*) * argc);

  for (i = 0; i < argc; i++)
  {
    if (argv[i]->type == VAR_CALL)
    {
      variable_array_t* array = (variable_array_t*) argv[i]->element;

      realValues[i] = eval(array->size, array->element, evnt);
    }
    else
    {
      realValues[i] = eval(1, argv + i, evnt);
    }
  }

  if (argc >= 2)
  {
    if (realValues[1]->type == VAR_FUNCTION && realValues[0]->type != VAR_FUNCTION)
    {
      variable_t* t = realValues[0];
      realValues[0] = realValues[1];
      realValues[1] = t;
    }
  }

  if (realValues[0]->type == VAR_FUNCTION)
  {
    variable_function_t* f = (variable_function_t*) realValues[0]->element;
    rValue = f->function(argc, realValues);
  }
  else if (argv[0]->type == VAR_SYMBOL)
  {
    char* funcName = *(char**) argv[0]->element;

    if (!strcmp(funcName, "matrix"))
      rValue = f_matrix(argc, realValues);
    else if (!strcmp(funcName, "rmatrix") || !strcmp(funcName, "randomMatrix"))
      rValue = f_matrix_random(argc, realValues);
    else if (!strcmp(funcName,"addition"))
      rValue = f_add(argc, realValues);
    else if (!strcmp(funcName,"mult"))
      rValue = f_mult(argc, realValues);
    else if (!strcmp(funcName, "get"))
      rValue = f_get(argc, realValues);
    else if (!strcmp(funcName, "echo"))
      rValue = f_echo(argc, realValues);
    else if (!strcmp(funcName,"mult_scal"))
      rValue = f_multScal(argc, realValues);
    else if (!strcmp(funcName,"expo"))
      rValue = f_pow(argc, realValues);
    else if (!strcmp(funcName,"transpose"))
      rValue = f_trans(argc, realValues);
    else if (!strcmp(funcName,"determinant"))
      rValue = f_det(argc, realValues);
    else if (!strcmp(funcName,"invert"))
      rValue = f_invert(argc, realValues);
    else if (!strcmp(funcName,"solve"))
      rValue = f_solve(argc, realValues);
    else if (!strcmp(funcName,"valP"))
      rValue = f_valP(argc, realValues);
    else if (!strcmp(funcName,"decomposition"))
      rValue = f_LU(argc, realValues);
    else if (!strcmp(funcName, "get"))
      rValue = f_get(argc, realValues);
    else
    {
      bleh("Trying to call unknown function.");
      fprintf(stderr, " -> function name: %s\n", funcName);

      rValue = varNew(VAR_NULL);
    }
  }

  for (i = 0; i < argc; i++)
  {
    varFree(realValues[i]);
  }

  free(realValues);

  if (rValue)
    return rValue;
  else
    return varNew(VAR_NULL);
}

