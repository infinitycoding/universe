/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the Ultrashell.
 
     The Ultrashell is free software: you can redistribute it and/or modify
     it under the terms of the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.
 
     The Ultrashell is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser General Public License for more details.
 
     You should have received a copy of the GNU Lesser General Public License
     along with the Ultrashell.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @author Peter Hösch aka. BlitzBasic <peter.hoesch@infinitycoding.de>
 **/



#include <ultrashell.h>
#include <basicCMDs.h>
#include <binaryTree.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/**
 * @brief the main function of the ultrashell.
 * @param argc the number of arguments (standart in main functions, currently ignored)
 * @param argv the arguments values (standart in main functions, currently ignored)
 * @return currently useless, because main never terminates
 */ 

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

	while(1);
	return 0;
}


/**
 * @brief creates the binary tree with the standart commands
 * @return a pointer to the created binary tree
 */ 

binary_tree *initBinaryTree()
{
	binary_tree *cmdTree = NULL;
	
	loadFunction(&cmdTree, "echo", echo);
	loadFunction(&cmdTree, "sver", sver);
	loadFunction(&cmdTree, "true", cmdtrue);
	loadFunction(&cmdTree, "false", cmdfalse);
	loadFunction(&cmdTree, "cd", cd);
	loadFunction(&cmdTree, "pwd", pwd);
	loadFunction(&cmdTree, "exit", sexit);

	return cmdTree;
}


/**
 * @brief parsers and executes a line (strange, with this name...)
 * @param state the current state of the shell in which the line should be executed
 * @param line the line which should be executed
 * @return 0 if the command in the line was executed
 * @return -1 if the command in the line is unknown
 */

int parserLine(struct shell_state *state, const char *line)
{
	char **argv = NULL;
	int argc = 0;
	struct function *fct = NULL;

	argc = getTokens(line, &argv);
	argc = replaceTokens(argc, &argv, state);

	if((fct = searchFunction(state->cmds, argv[0])) != NULL)
	{
		state->last_ret_value = fct->f(argc, argv);

		for(; argc > 0; argc--)
			free(argv[argc-1]);

		return 0;
	}
	else
	{
		printf("%s : unknown filename or command.\n", argv[0]);
		return -1;	
	}
}


/**
 * @brief separates a string into tokens
 * @param instring the string which should be separated
 * @param tokens the space in which the tokens should be placed
 * @return the number of the tokens
 */

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


/**
 * @brief replaces tokens in a string with other things (vars, ...)
 * @param number the number of the tokens until now
 * @param tokens the tokens until now
 * @param state the state of the shell this function is executed in
 * @return the number of the token now
 */

int replaceTokens(int number, char ***tokens, struct shell_state *state)
{
	int i;

	for(i = 0; i < number; i++)
	{
		if(!strncmp(tokens[0][i], "$?", 2))
		{
			free(tokens[0][i]);
			char tmpstring[20];
			sprintf(tmpstring, "%d", state->last_ret_value);
			int length = strlen(tmpstring);
			tokens[0][i] = (char *)malloc(sizeof(char) * (length + 1));
			strncpy(tokens[0][i], tmpstring, length);
		}
	}

	return number;
}


/**
 * @brief counts the number of tokens in a string
 * @param instring the string in which the tokens should be counted
 * @return the number of the tokens in the string
 */

int countTokens(const char *instring)
{
	int tokenNumber = 1;
	int i = 0;

	for(i = 0; instring[i] != '\n' && instring[i] != '\0'; i++)
		if(instring[i] == ' ' && instring[i + 1] != ' ')
			tokenNumber++;

	return tokenNumber;
}
