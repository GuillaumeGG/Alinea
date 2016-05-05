// Fichier des variables
// Auteur : Wasmer Audric et Grosshenny Guillaume

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

void varPrint(variable_t* var, int indent);

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
			int size_mem = size_elem(var->type);
			memcpy(varCp->element, var->element, size_mem);
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
		case VAR_NULL :
			varCp = varNew(VAR_NULL);
			break;
	}

	return varCp;
}



int offsetSymbol(char* input, int offsetCur)
{
  int i=offsetCur;
  for(;(isalnum(input[i])) || (input[i] == '_') || (input[i] == ':') ;i++);
  ;
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

			for (n = 0; n < strlen(buffer) - 5; n++)
				printf(" ");

			printf("%s ", buffer);
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
    if(isalpha(input[*offset]) || input[*offset] == '_' || input[*offset] == ':')
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

	printf(" >> Building [%i:%i] matrix.\n", width, array->size);

	output = varNew(VAR_MATRIX);
	outputMatrix = newMatrix(array->size, width);
	memcpy(output->element, &outputMatrix, sizeof(outputMatrix));

	for (i = 0; i < array->size; i++)
	{
		int j;
		variable_array_t* line = (variable_array_t*) array->element[i]->element;

		for (j = 0; j < line->size; j++)
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
  
  result_add = additionMatrix(mat1_value, mat2_value);
  
  output = varNew(VAR_MATRIX);
  memcpy(output->element, &result_add, sizeof(result_add));

  return output;
}

variable_t* f_mult(int argc, variable_t** argv)
{
  variable_array_t* array_mat;
  variable_t *mat1, *mat2;
  variable_t *output;
  struct matrix *mat1_value, *mat2_value, *result_mult;
  
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
  
  result_mult = multiplicationMatrix(mat1_value, mat2_value);
  
  output = varNew(VAR_MATRIX);
  memcpy(output->element, &result_mult, sizeof(result_mult));

  return output;
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
			variable_array_t* array = (variable_array_t*) realValues[i]->element;

			realValues[i] = eval(array->size, array->element, evnt);
		}
		else
		{
			realValues[i] = eval(1, argv + i, evnt);
		}
	}

	if (argv[0]->type == VAR_SYMBOL)
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

