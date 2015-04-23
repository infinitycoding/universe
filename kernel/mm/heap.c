/*
     Copyright 2015 Infinitycoding all rights reserved
     This file is part of the Universe Kernel.

     The Universe Kernel is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.

     The Universe Kernel is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with the Universe Kernel. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @author Michael Sippel <micha@infinitycoding.de>
 */
#include <mm/heap.h>
#include <stdint.h>
#include <stddef.h>

void heap_create(heap_t *heap, heap_node_t* (*node_create)(void), void (*node_destroy)(heap_node_t*))
{
	heap->used_root = NULL;
	heap->free_root = NULL;

	heap->node_create = node_create;
	heap->node_destroy= node_destroy;
}

heap_node_t* heap_alloc(heap_t* heap, size_t length)
{
	heap_node_t** found_node = heap_find_free(heap, length);
	heap_node_t* node = *found_node;

	if(node->length > length)
	{
		uintptr_t base = node->base;

		// shrink old
		node->base += length;
		node->length -= length;

		// create new
		heap_node_t *new_node = heap->node_create();
		new_node->base = base;
		new_node->length = length;
		new_node->bottom = node->bottom;
		new_node->top = new_node;
		node->bottom = new_node;

		node = new_node;
	}
	else
		heap_remove(found_node);

	node->flags &= ~HEAP_MERGE;
	heap_insert_used(heap, node);

	return node;
}

heap_node_t* heap_free(heap_t* heap, uintptr_t base)
{
	heap_node_t** found_node = heap_find_used(heap, base);
	heap_node_t* node = *found_node;

	if(node != NULL)
	{
		node->flags |= HEAP_MERGE;
		heap_remove(found_node);
		heap_insert_free(heap, node);
	}

	return node;
}

heap_node_t** heap_find_used(heap_t* heap, uintptr_t base)
{
	heap_node_t** node = &heap->used_root;
	while((*node) != NULL)
	{
		if(base == (*node)->base)
			break;

		if(base < (*node)->base)
			node = &(*node)->left;
		else
			node = &(*node)->right;
	}

	return node;
}

heap_node_t** heap_find_free(heap_t* heap, size_t length)
{
	heap_node_t** node = &heap->free_root;
	heap_node_t** best = NULL;

	while((*node) != NULL)
	{
		if(best == NULL || (*node)->length < (*best)->length)
			best = node;

		if(length < (*node)->length)
		{
			node = &(*node)->left;
		}
		else
			node = &(*node)->right;
	}

	return best;
}

heap_node_t** heap_insert_used(heap_t* heap, heap_node_t* new_node)
{
	new_node->left = NULL;
	new_node->right = NULL;
	heap_node_t** node = NULL;

//	if(new_node->flags & HEAP_MERGE)
//		node = heap_merge(heap, new_node);

	if(node == NULL)
	{
		node = &heap->used_root;
		while((*node) != NULL)
		{
			if(new_node->base < (*node)->base)
				node = &(*node)->left;
			else
				node = &(*node)->right;
		}
		(*node) = new_node;
	}

	return node;
}

heap_node_t** heap_insert_free(heap_t* heap, heap_node_t* new_node)
{
	new_node->left = NULL;
	new_node->right = NULL;
	heap_node_t** node = NULL;

//	if(new_node->flags & HEAP_MERGE)
//		node = heap_merge(heap, new_node);

	if(node == NULL)
	{
		node = &heap->free_root;
		while((*node) != NULL)
		{
			if(new_node->length <= (*node)->length)
				node = &(*node)->left;
			else
				node = &(*node)->right;
		}
		(*node) = new_node;
	}

	return node;
}

int heap_height(heap_node_t *root)
{
	int hl = 0, hr = 0;

	if(root->left != NULL)
		hl = heap_height(root->left);

	if(root->right != NULL)
		hl = heap_height(root->right);

	return (1 + ((hl > hr) ? hl : hr));
}
/*
heap_node_t** heap_merge(heap_t* heap, heap_node_t* node)
{
	heap_node_t* old = NULL;

	if(node->top != NULL &&
		(node->top->flags & HEAP_MERGE) &&
		node->top->base == (node->base + node->length))
	{
		// merge node with top
		node->top->base = node->base;
		node->top->length += node->length;
		node->top->flags |= node->flags;
		node->top->bottom = node->bottom;
		if((*node)->bottom != NULL)
			(*node)->bottom->top = (*node)->top;

		old = node;
		node = node->top;
		heap->node_destroy(old);
	}

	if(node->bottom != NULL &&
		(node->bottom->flags & HEAP_MERGE) &&
		(node->bottom->base + node->bottom->length) == node->base)
	{
		// merge node with bottom
		node->bottom->length += node->length;
		node->bottom->flags |= node->flags;
		node->bottom->top = node->top;
		if(node->top != NULL)
			node->top->bottom = node->bottom;

		if(old != NULL)
			heap_remove(&node->bottom);

		old = node;
		node = node->bottom;
		heap->node_destroy(old);
	}

	return node;
}
*/
void heap_remove(heap_node_t** node)
{
	heap_node_t** swap = NULL;
	heap_node_t* replace;

	// select inner leave
	if((*node)->left != NULL)
	{
		swap = &(*node)->left;
		while((*swap)->right != NULL)
			swap = &(*swap)->right;

		replace = (*swap);

		// remove node
		(*swap) = (*swap)->left;
	}
	else if((*node)->right != NULL)
	{
		swap = &(*node)->right;
		while((*swap)->left != NULL)
			swap = &(*swap)->left;

		replace = (*swap);
		printf("swap: %x\n", replace->base);

		// remove node
		(*swap) = (*swap)->right;
	}
	else
	{
		(*node) = NULL;
		return;
	}

	replace->left = (*node)->left;
	replace->right = (*node)->right;
	(*node) = replace;
}

