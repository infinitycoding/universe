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
	binary_tree *cmds = initBinaryTree();
	
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
			parserLine(cmds, inbuffer);

			counter = 0;
			inbuffer[counter] = '\0';
		}
	}

	return 0;
}


// creates the binary tree with the commands

binary_tree *initBinaryTree()
{
	binary_tree *cmdTree = NULL;

	struct function *newCmd = (struct function *) malloc(sizeof(struct function));
	newCmd->command = "dummy";
	newCmd->f = dummy;

	cmdTree = create_tree(newCmd);
	
	newCmd = (struct function *) malloc(sizeof(struct function));
	newCmd->command = "echo";
	newCmd->f = echo;
	addFunction(cmdTree, newCmd);

	newCmd = (struct function *) malloc(sizeof(struct function));
	newCmd->command = "sver";
	newCmd->f = sver;
	addFunction(cmdTree, newCmd);

	newCmd = (struct function *) malloc(sizeof(struct function));
	newCmd->command = "true";
	newCmd->f = cmdtrue;
	addFunction(cmdTree, newCmd);

	newCmd = (struct function *) malloc(sizeof(struct function));
	newCmd->command = "false";
	newCmd->f = cmdfalse;
	addFunction(cmdTree, newCmd);

	newCmd = (struct function *) malloc(sizeof(struct function));
	newCmd->command = "add";
	newCmd->f = add;
	addFunction(cmdTree, newCmd);

	newCmd = (struct function *) malloc(sizeof(struct function));
	newCmd->command = "sub";
	newCmd->f = sub;
	addFunction(cmdTree, newCmd);

	newCmd = (struct function *) malloc(sizeof(struct function));
	newCmd->command = "mul";
	newCmd->f = mul;
	addFunction(cmdTree, newCmd);

	newCmd = (struct function *) malloc(sizeof(struct function));
	newCmd->command = "div";
	newCmd->f = div;
	addFunction(cmdTree, newCmd);

	return cmdTree;
}


// parsers and executes a line (strange, with this name...)

int parserLine(binary_tree *tree, const char *line)
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
