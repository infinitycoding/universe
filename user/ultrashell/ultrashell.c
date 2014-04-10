/*
     Copyright 2014 Infinitycoding all rights reserved
     This file is part of the Universe Kernel.
 
     Universe Kernel is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.
 
     Universe Kernel is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
 
     You should have received a copy of the GNU General Public License
     along with Universe Kernel.  If not, see <http://www.gnu.org/licenses/>.
 */



#include <ultrashell.h>
#include <basicCMDs.h>
#include <binaryTree.h>

#include <universe.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


// main loop which pulls the inputs and tells the other functions what to do 

int main(int argc, char **argv)
{
	printf("Ultrashell started.\n");
	struct shell_state shell;	
	shell.last_ret_value = 0;
	shell.cmds = initBinaryTree();

	int counter = 0;
	char inchar;
	char inbuffer[MAX_LINE_LENGTH] = "";	
	
	do
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
			parserLine(&shell, inbuffer);

			counter = 0;
			inbuffer[counter] = '\0';
		}
	}FOREVER;

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
	
	loadFunction(cmdTree, "echo", echo);
	loadFunction(cmdTree, "sver", sver);
	loadFunction(cmdTree, "true", cmdtrue);
	loadFunction(cmdTree, "false", cmdfalse);
	loadFunction(cmdTree, "cd", cd);
	loadFunction(cmdTree, "pwd", pwd);
	loadFunction(cmdTree, "add", add);
	loadFunction(cmdTree, "sub", sub);
	loadFunction(cmdTree, "mul", mul);
	loadFunction(cmdTree, "div", div);

	return cmdTree;
}


// parsers and executes a line (strange, with this name...)

int parserLine(struct shell_state *state, const char *line)
{
	char **argv = NULL;
	int argc = 0;
	struct function *fct = NULL;

	argc = getTokens(line, &argv);

	if((fct = searchFunction(state->cmds, argv[0])) != NULL)
	{
		state->last_ret_value = fct->f(argc, argv);
		return 0;
	}
	else
	{
		printf("%s : unknown filename or command.\n", argv[0]);
		return -1;	
	}
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
