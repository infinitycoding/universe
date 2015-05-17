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
 *
 *	!!! WARNING !!!
 *	THIS IS A FUCKING POINTER HACK
 *  Glory to them who debug it ;)
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

void heap_update_pointers(heap_node_t* n, heap_node_t** ptr)
{
    if(n != NULL)
    {
        if(n->bottom != NULL && *n->bottom != NULL)
            (*n->bottom)->top = ptr;

        if(n->top != NULL && *n->top != NULL)
            (*n->top)->bottom = ptr;
    }
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

        // insert into linear chain
        new_node->bottom = node->bottom;
        new_node->top = found_node;

        // insert into used tree; next wee need 'ins'
        heap_node_t** ins = heap_insert_used(heap, new_node);

        // rest linear chain
        node->bottom = ins;

        if(new_node->bottom != NULL && *new_node->bottom != NULL)
            (*new_node->bottom)->top = ins;

        if(heap->bottom == node)
            heap->bottom = new_node;

        if(heap->top == node)
            heap->top = new_node;

        node = new_node;
    }
    else
    {
        // remove from free tree
        heap_remove(found_node);

        // insert into used tree
        heap_insert_used(heap, node);
    }

    return node;
}

heap_node_t* heap_free(heap_t* heap, uintptr_t base)
{
    heap_node_t** found_node = heap_find_used(heap, base);
    heap_node_t* node = *found_node;

    if(node != NULL)
    {
        // remove from used tree
        heap_remove(found_node);

        // insert to free tree
        heap_node_t** ins = heap_insert_free(heap, node);
        node = *ins;

        // merge
        if(node->bottom != NULL)
            heap_merge(heap, node->bottom, ins);

        if(node->top != NULL)
            heap_merge(heap, ins, node->top);
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
        if(best == NULL || ( (*node)->length < (*best)->length && (*node)->length >= length ) )
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

    heap_node_t** node = &heap->used_root;
    while((*node) != NULL)
    {
        if(new_node->base < (*node)->base)
            node = &(*node)->left;
        else
            node = &(*node)->right;
    }

    heap_update_pointers(new_node, node);
    (*node) = new_node;

    new_node->flags &= ~HEAP_FREE;

    return node;
}

heap_node_t** heap_insert_free(heap_t* heap, heap_node_t* new_node)
{
    new_node->left = NULL;
    new_node->right = NULL;

    heap_node_t** node = &heap->free_root;
    while((*node) != NULL)
    {
        if(new_node->length <= (*node)->length)
            node = &(*node)->left;
        else
            node = &(*node)->right;
    }

    heap_update_pointers(new_node, node);
    (*node) = new_node;

    new_node->flags |= HEAP_FREE;

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

heap_node_t** heap_merge(heap_t* heap, heap_node_t** node1, heap_node_t** node2)
{
    heap_node_t *n1 = *node1;
    heap_node_t *n2 = *node2;

    if(n1 != NULL && n2 != NULL && n1->flags == n2->flags && (n1->flags & HEAP_MERGE))
    {
        if(n2->base == (n1->base + n1->length))
        {
            n1->length += n2->length;
            n1->flags |= n2->flags;

            heap_remove(node2);

            n1->top = n2->top;
            if(n1->top != NULL && *n1->top != NULL)
                (*n1->top)->bottom = node1;

            heap->node_destroy(n2);
        }
    }

    return node1;
}

void heap_remove(heap_node_t** node)
{
    heap_node_t* n = *node; // only to write less
    heap_node_t** swap = NULL; // the node to swap with
    heap_node_t* s = NULL;

    // select inner leaf
    if(n->left != NULL)
    {
        swap = &n->left;
        while((*swap)->right != NULL)
            swap = &(*swap)->right;
    }
    else if(n->right != NULL)
    {
        swap = &n->right;
        while((*swap)->left != NULL)
            swap = &(*swap)->left;
    }
    else
        goto end;

    s = *swap;

    // remove the inner leaf from tree
    if(n->left != NULL)
        (*swap) = s->left;
    else
        (*swap) = s->right;

    // replace node with inner leaf
    s->left = n->left;
    s->right = n->right;
    heap_update_pointers(s, node);

end:
    (*node) = s;

    // keep node in linear chain
    n->left = n; // node will be removed from tree, so we use the own left pointer
    heap_update_pointers(n, &n->left);
}

