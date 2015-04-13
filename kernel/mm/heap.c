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
    heap_node_t* node = heap_find_size(heap->free_root, length);

    if(node->length > length)
    {
        uintptr_t base = node->base;

        // shrink old
        node->base += length;
        node->length -= length;

        // create new
        node = heap->node_create();
        node->base = base;
        node->length = length;
    }
    else
        node = heap_remove(node);

    heap_insert_base(&heap->used_root, node);

    return node;
}

heap_node_t* heap_free(heap_t* heap, uintptr_t base)
{
    heap_node_t *node = heap_find_base(heap->used_root, base);
    node = heap_remove(node);

    // TODO: merge
    heap_insert_size(&heap->free_root, node);

    return node;
}

heap_node_t* heap_find_base(heap_node_t* root, uintptr_t base)
{
    heap_node_t* node = root;
    while(node != NULL)
    {
        if(base == node->base)
            break;

        if(base < node->base)
            node = node->left;
        else
            node = node->right;
    }

    return node;
}

heap_node_t* heap_find_size(heap_node_t* root, size_t length)
{
    heap_node_t* node = root;
    heap_node_t* best = NULL;

    while(node != NULL)
    {
        if(length <= node->length)
        {
            if(best == NULL || node->length < best->length)
                best = node;

            node = node->left;
        }
        else
            node = node->right;
    }

    return best;
}

void heap_insert_base(heap_node_t** root, heap_node_t* new_node)
{
    heap_node_t** node = root;
    while((*node) != NULL)
    {
        if(new_node->base < (*node)->base)
            node = &(*node)->left;
        else
            node = &(*node)->right;
    }

    new_node->left = NULL;
    new_node->right = NULL;
    (*node) = new_node;
}

void heap_insert_size(heap_node_t** root, heap_node_t* new_node)
{
    heap_node_t** node = root;
    while((*node) != NULL)
    {
        if(new_node->length <= (*node)->length)
            node = &(*node)->left;
        else
            node = &(*node)->right;
    }

    new_node->left = NULL;
    new_node->right = NULL;
    (*node) = new_node;
}

heap_node_t* heap_remove(heap_node_t* node)
{
    heap_node_t** swap = &node->right;
    while((*swap)->left != NULL)
        swap = &(*swap)->left;

    heap_node_t tmp;
    tmp.base = node->base;
    tmp.length = node->length;
    tmp.flags = node->flags;

    node->base = (*swap)->base;
    node->length = (*swap)->length;
    node->flags = (*swap)->flags;

    node = (*swap);
    node->base = tmp.base;
    node->length = tmp.length;
    node->flags = tmp.flags;

    (*swap) = (*swap)->right;

    return node;
}

