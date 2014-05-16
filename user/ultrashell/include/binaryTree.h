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



#ifndef BINARYTREE_H_INCLUDED
#define BINARYTREE_H_INCLUDED



#define binary_tree struct branch

#define MAX_COMMAND_LENGTH 20

#define SUCCESS 0
#define BINARY_TREE_CORRUPTED -1
#define COMMAND_ALREADY_ADDED -2
#define ALLOCATE_ERROR -3



enum branchType
{
	moreBranching, branchEnd
};


struct condition	// not sure if i really want to make it like this
{
	int number;
	char value;
};


struct function
{
	char *command;
	int (*f)(int, char **);
};


struct branching
{
	struct condition *c;
	struct branch *left;
	struct branch *right;
};


struct branch
{
	enum branchType type;
	union branchElement *element;
};


union branchElement
{
	struct branching *b;
	struct function *f;
};



void deleteTree(binary_tree *tree);
void deleteBranch(struct branch *b);
int loadFunction(binary_tree **tree, char *cmd, int (*fct)(int, char **));
int addFunction(binary_tree **tree, struct function *new_function);
struct function *searchFunction(binary_tree *tree, char *function_name);



#endif
