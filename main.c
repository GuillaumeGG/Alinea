// Fichier main du projet
// Auteurs : Wasmer Audric et Grosshenny Guillaume

#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "variable.h"

int isfullspace(char* s)
{
	int i;

	for (i = 0; s[i]; i++)
	{
		if (!isspace(s[i]))
			return 0;
	}

	return 1;
}

int main(int argc, char **argv)
{
	char* input;
	environment_t* evnt = NULL;

	(void) argc;
	(void) argv;

	for(;;)
	{
		variable_t* rValue;

		if (0)
			input = readline("Mini-CAS > "); // Affiche le prompt et récupère l'entrée
		else
			input = readline("\001\033[01;37;44m\002minicas\001\033[00;34;46m\002\001\033[00;36m\002\001\033[00m\002 ");

		if(input == NULL) // Fin de fichier
		{
			break;
		}

		if (isfullspace(input))
		{
			free(input);
			continue;
		}

		add_history(input);

		/* ---- gestion dee l'input ---- */
		variable_t* varTest;
		variable_array_t* arg;

		varTest = varParser(input);
		arg = (variable_array_t*)varTest->element;

		//printf("VARIABLE : ");
		//varPrint(varTest, 0);

		rValue = eval(arg->size, arg->element, &evnt);

		varPrint(rValue, 0);

		/* ---- fin de gestion de l'input ---- */
		free(input);
		//varPrint(varTest, 0);
		varFree(varTest);
		varFree(rValue);
	}

	freeEvt(evnt);

	return 0;
}




