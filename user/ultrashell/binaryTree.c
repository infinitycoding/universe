#include <binaryTree.h>

#include <stdlib.h>
#include <string.h>



binary_tree *create_tree(struct function *dummy)
{
	binary_tree *tree = NULL;

	tree = (binary_tree *) malloc(sizeof(binary_tree));
	tree->type = branchEnd;
	tree->element = (union branchElement *) malloc(sizeof(union branchElement));
	tree->element->f = dummy;
	
	return tree;
}


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


int addFunction(binary_tree *tree, struct function *new_function)
{
	int i = 0;
	struct branch *current_node = tree;
	union branchElement *newElement = NULL;
	union branchElement *oldElement = NULL;


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
