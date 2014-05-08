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



#ifndef ULTRASHELL_H
#define ULTRASHELL_H



#include <binaryTree.h>



#define FOREVER while(1)
#define MAX_PATH_LENGTH 100
#define MAX_LINE_LENGTH 50
#define MAX_TOKEN_LENGTH 20



struct shell_state
{
	int last_ret_value;
	binary_tree *cmds;
};



int main(int argc, char **argv);
binary_tree *initBinaryTree();
int parserLine(struct shell_state *state, const char *line);
int getTokens(const char *instring, char ***tokens);
int countTokens(const char *instring);



#endif
