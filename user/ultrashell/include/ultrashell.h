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



#ifndef ULTRASHELL_H
#define ULTRASHELL_H



#include <binaryTree.h>



#define FOREVER while(1)
#define MAX_PATH_LENGTH 100
#define MAX_LINE_LENGTH 50
#define MAX_TOKEN_LENGTH 20



struct shell_state
{
	binary_tree *cmds;
	char working_directory[MAX_PATH_LENGTH];
};



int main(void);
binary_tree *initBinaryTree();
int parserLine(binary_tree *tree, const char *line);
int getTokens(const char *instring, char ***tokens);
int countTokens(const char *instring);

#endif
