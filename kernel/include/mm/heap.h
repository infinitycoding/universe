#ifndef _heap_h_
#define _heap_h_
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
#include <stdint.h>

typedef struct heap_node
{
    uintptr_t base;
    size_t length;
    uint8_t flags;

    // tree pointers
    struct heap_node* left;
    struct heap_node* right;

    struct heap_node** top;
    struct heap_node** bottom;
} heap_node_t;

typedef struct heap
{
    heap_node_t* free_root;
    heap_node_t* used_root;

    heap_node_t* top;
    heap_node_t* bottom;

    heap_node_t* (*node_create)(void);
    void (*node_destroy)(heap_node_t* node);
} heap_t;

void heap_create(heap_t *heap, heap_node_t* (*node_create)(void), void (*node_destroy)(heap_node_t*));

// heap functions
heap_node_t* heap_alloc(heap_t* heap, size_t size);
heap_node_t* heap_free(heap_t* heap, uintptr_t base);

// tree functions
heap_node_t** heap_find_used(heap_t* heap, uintptr_t base);
heap_node_t** heap_find_free(heap_t* heap, size_t length);
heap_node_t** heap_insert_used(heap_t* heap, heap_node_t* new_node);
heap_node_t** heap_insert_free(heap_t* heap, heap_node_t* new_node);
heap_node_t** heap_merge(heap_t* heap, heap_node_t** node1, heap_node_t** node2);
void heap_remove(heap_node_t** node);

#endif

