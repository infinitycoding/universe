#include <ultrashell.h>
#include <basicCMDs.h>

#include <universe.h>
#include <string.h>
#include <stdlib.h>



// main is needed to compile, currently test only

int main(void)
{
	parserLine("sver\n");
	parserLine("echo 13 * 17 =\n");
	parserLine("mul 13 17\n");

	while(1) {
        printf("%c",getchar());
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
	else if(!strncmp(argv[0], "mul", 3))
		return mul(argc, argv);
	else
		printf("Unknown filename or command.\n");

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
