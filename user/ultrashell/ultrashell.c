#include <ultrashell.h>

#include <universe.h>



// main is needed to compile, currently unused

int main(void)
{
	//char **args = NULL;
	//int argc = 0;
	//int i = 0;

	print("Ultrashell!\n");

	/*argc = getTokens("git add .\n", args);

	for(i = 0; i < argc; i++)
	{
		print(args[i]);
		print('\n');
	}*/
	return 0;
}


// separates a string into tokens
// unworkable because of missing malloc

/*int getTokens(const char *instring, char ***tokens)
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
			tokens[0][currentTokenNumber] = (char *)malloc(sizeof(char) * tokenSize);
			strncpy(tokens[0][currentTokenNumber], currentToken, MAX_TOKEN_LENGTH);

			memset(currentToken, '\0', sizeof(currentToken));
			currentTokenNumber++;
			tokenSize = 0;
		}
		else
		{
			currentToken[tokenSize - 1] = instring[i];
			tokenSize++;
		}
	}

	return tokenNumber;
}*/


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
