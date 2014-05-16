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



#include <binaryTree.h>

#include <stdlib.h>
#include <string.h>



/**
 * @brief deletes a binary tree
 * @param tree the tree to delete
 * @return no return value
 */

void deleteTree(binary_tree *tree)
{
	deleteBranch((struct branch *)tree);
}


/**
 * @brief deletes a branch of a binary tree and all it's subbranches
 * @param b the branch which should be deleted
 * @return no return value
 */

void deleteBranch(struct branch *b)
{
	if(b->type == branchEnd)
	{
		free(b->element->f->command);
		free(b->element->f);
	}
	else
	{
		deleteBranch(b->element->b->left);
		deleteBranch(b->element->b->right);

		free(b->element->b->c);
		free(b->element->b);
	}

	free(b->element);
	free(b);
}


/**
 * @brief creates a function struct in the ram and adds the function to a binary tree
 * @param tree the tree in which the new function should be added
 * @param cmd the command with which the function should be called later on
 * @param fct a pointer to the function which should be added to the tree
 * @return 0 = success
 * @return -1 = binary tree corrupted
 * @return -2 = command allready added
 * @return -3 = allocate error
 */

int loadFunction(binary_tree **tree, char *cmd, int (*fct)(int, char **))
{
	struct function *newFunction = NULL;
	newFunction = (struct function *) malloc(sizeof(struct function));

	if(newFunction == NULL)
		return ALLOCATE_ERROR;

	newFunction->command = cmd;
	newFunction->f = fct;

	return addFunction(tree, newFunction);
}


/**
 * @brief adds a new function to a binary tree
 * @param tree the tree in which the new function should be added
 * @param new_function the function struct which should be added to the binary tree
 * @return 0 = success
 * @return -1 = binary tree corrupted
 * @return -2 = command allready added
 */

int addFunction(binary_tree **tree, struct function *new_function)
{
	int i = 0;
	struct branch *current_node = (*tree);
	union branchElement *newElement = NULL;
	union branchElement *oldElement = NULL;

	if((*tree) == NULL)
	{
		(*tree) = (binary_tree *) malloc(sizeof(binary_tree));
		(*tree)->type = branchEnd;
		(*tree)->element = (union branchElement *) malloc(sizeof(union branchElement));
		(*tree)->element->f = new_function;

		return SUCCESS;
	}

	while(current_node != NULL && current_node->type != branchEnd)
	{
		if(new_function->command[current_node->element->b->c->number] == current_node->element->b->c->value)
			current_node = current_node->element->b->left;
		else
			current_node = current_node->element->b->right;
	}

	if(current_node == NULL)
	{
		return BINARY_TREE_CORRUPTED;
	}	
	else if(!strncmp(current_node->element->f->command, new_function->command, MAX_COMMAND_LENGTH))
	{	
		return COMMAND_ALREADY_ADDED;
	}	
	else
	{
		for(i = 0; current_node->element->f->command[i] == new_function->command[i]; i++);

		current_node->type = moreBranching;
		oldElement = current_node->element;
		newElement = (union branchElement *) malloc(sizeof(union branchElement));
		current_node->element = newElement;
		current_node->element->b = (struct branching *) malloc(sizeof(struct branching));
		current_node->element->b->c = (struct condition *) malloc(sizeof(struct condition));
		current_node->element->b->left = (struct branch *) malloc(sizeof(struct branch));
		current_node->element->b->right = (struct branch *) malloc(sizeof(struct branch));
		current_node->element->b->left->type = branchEnd;
		current_node->element->b->right->type = branchEnd;
		current_node->element->b->c->number = i;
		current_node->element->b->c->value = new_function->command[i];
		current_node->element->b->left->element = (union branchElement *) malloc(sizeof(union branchElement));
		current_node->element->b->left->element->f = new_function;
		current_node->element->b->right->element = oldElement;

		return SUCCESS;
	}
}


/**
 * @brief returns you a function out of a binary tree
 * @param tree the tree in which the function should be searched
 * @param function_name the name of the function which should be searched
 * @return the function struct of the function
 */

struct function *searchFunction(binary_tree *tree, char *function_name)
{	
	struct branch *current_node = tree;

	while(current_node != NULL && current_node->type != branchEnd)
	{
		if(current_node->type == moreBranching)
		{
			if(function_name[current_node->element->b->c->number] == current_node->element->b->c->value)
			{
				current_node = current_node->element->b->left;
			}			
			else
			{
				current_node = current_node->element->b->right;
			}
		}
	}

	if(current_node == NULL)
	{
		return NULL;
	}	
	else if(strncmp(current_node->element->f->command, function_name, MAX_COMMAND_LENGTH))
	{
		return NULL;
	}
	else
	{
		return current_node->element->f;
	}
}
