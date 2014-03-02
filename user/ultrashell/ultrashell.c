#include <ultrashell.h>
#include <basicCMDs.h>

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
			parserLine(inbuffer);

			counter = 0;
			inbuffer[counter] = '\0';
		}
	}

	return 0;
}


// parsers a line (strange, with this name...)

int parserLine(const char *line)
{
	char **argv = NULL;
	int argc = 0;

	argc = getTokens(line, &argv);

	// TODO:
	// write from here (next lines are test only)

	if(!strncmp(argv[0], "echo", 4))
		return echo(argc, argv);
	else if(!strncmp(argv[0], "sver", 4))
		return sver(argc, argv);
	else if(!strncmp(argv[0], "true", 4))
		return cmdtrue(argc, argv);
	else if(!strncmp(argv[0], "false", 5))
		return cmdfalse(argc, argv);
	else if(!strncmp(argv[0], "add", 3))
		return add(argc, argv);
	else if(!strncmp(argv[0], "sub", 3))
		return sub(argc, argv);
	else if(!strncmp(argv[0], "mul", 3))
		return mul(argc, argv);
	else if(!strncmp(argv[0], "div", 3))
		return div(argc, argv);
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
