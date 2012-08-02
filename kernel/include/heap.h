#ifndef _heap_h_
#define _heap_h_

/*
 Copyright 2012 universe coding group (UCG) all rights reserved
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



 Diese Datei ist ein Teil vom Universe Kernel.

 Das Universe Kernel ist Freie Software: Sie koennen es unter den Bedingungen
 der GNU General Public License, wie von der Free Software Foundation,
 Version 3 der Lizenz oder jeder sp‰teren
 veroeffentlichten Version, weiterverbreiten und/oder modifizieren.

 Das Universe Kernel wird in der Hoffnung, dass es nuetzlich sein wird, aber
 Universe Kernel wird in der Hoffnung, dass es nuetzlich sein wird, aber
 OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 Gew‰hrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN ZWECK.
 Siehe die GNU General Public License fuer weitere Details.

 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/ 

#include <stdint.h>

/*
	cstdlib interface
*/

void * malloc ( size_t size );
void free ( void * ptr );
void * calloc ( size_t num, size_t size );
void * realloc ( void * ptr, size_t size );

/*
	internal interface
*/

struct alloc_t
{
	int size;
	
	struct alloc_t *prev;
	struct alloc_t *next;
};

typedef struct {
	void *pages[1];
	int page_count;
	
	struct alloc_t *alloc_list;
} heap_t;

void heap_init(heap_t *heap);
void heap_destroy(heap_t *heap);
void * heap_alloc(heap_t *heap, size_t size);
void * heap_free(heap_t *heap, void *ptr);
void * heap_realloc(heap_t *heap, void *ptr, size_t size);

#endif
