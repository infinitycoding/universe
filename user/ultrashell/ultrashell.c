#include <ultrashell.h>
#include <basicCMDs.h>
#include <binaryTree.h>

#include <universe.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// main is needed to compile, currently test only

int main(void)
{
	int counter = 0;
	char inchar;
	char inbuffer[MAX_LINE_LENGTH] = "";
	struct function *first_cmd = (struct function *) malloc(sizeof(struct function));
	first_cmd->command = "sver";
	first_cmd->f = sver;
	binary_tree *cmds = create_tree(first_cmd);
	
	while(1)
	{
		inchar = getchar();
		printf("%c", inchar);

		switch(inchar)
		{
			case '\0':
			break;

			case '\b':
				inbuffer[--counter] = '\0';
			break;

			default:
				inbuffer[counter++] = inchar;
				inbuffer[counter] = '\0';
			break;
		}

        	if(inchar == '\n')
		{
			parserLine(inbuffer, cmds);

			counter = 0;
			inbuffer[counter] = '\0';
		}
	}

	return 0;
}


// parsers and executes a line (strange, with this name...)

int parserLine(const char *line, binary_tree *tree)
{
	char **argv = NULL;
	int argc = 0;
	struct function *fct = NULL;

	argc = getTokens(line, &argv);

	if((fct = searchFunction(tree, argv[0])) != NULL)
		fct->f(argc, argv);
	else
		printf("%s : unknown filename or command.\n", argv[0]);

	return -1;
}


// separates a string into tokens

int getTokens(const char *instring, char ***tokens)
{
	char currentToken[MAX_TOKEN_LENGTH];
	int currentTokenNumber = 0;
	int tokenSize = 1;
	int tokenNumber = countTokens(instring);
	int i = 0;

	memset(currentToken, '\0', sizeof(currentToken));

	*tokens = (char **)malloc(sizeof(char *) * tokenNumber);

	for(i = 0; instring[i] != '\n' && instring[i] != '\0'; i++)
	{
		if(instring[i] == ' ')
		{
			while(instring[i + 1] == ' ')
				i++;

			currentToken[tokenSize] = '\0';
			tokens[0][currentTokenNumber] = (char *)malloc(sizeof(char) * (tokenSize+1));
			strncpy(tokens[0][currentTokenNumber], currentToken, MAX_TOKEN_LENGTH);

			memset(currentToken, '\0', sizeof(currentToken));
			currentTokenNumber++;
			tokenSize = 1;
		}
		else
		{
			currentToken[tokenSize - 1] = instring[i];
			tokenSize++;
		}
	}

	currentToken[tokenSize] = '\0';
	tokens[0][currentTokenNumber] = (char *)malloc(sizeof(char) * (tokenSize+1));
	strncpy(tokens[0][currentTokenNumber], currentToken, MAX_TOKEN_LENGTH);

	return tokenNumber;
}


// counts the number of tokens in a string

int countTokens(const char *instring)
{
	int tokenNumber = 1;
	int i = 0;

	for(i = 0; instring[i] != '\n' && instring[i] != '\0'; i++)
		if(instring[i] == ' ' && instring[i + 1] != ' ')
			tokenNumber++;

	return tokenNumber;
}
