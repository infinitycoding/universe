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

    Das Universe Kernel ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder jeder späteren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Das Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEWÄHELEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License für weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/

/**
	@author Tom Slawik <tom.slawik@gmail.com>
	
	improvisoric :D will be rewritten in the next days
*/

#include <stdint.h>
#include <pmm.h>
#include <paging.h>

struct mem_header {
	size_t size;
	struct mem_header *prev;
	struct mem_header *next;
};

static void *heap;
static struct mem_header *mlist_head;

void INIT_MALLOC(void)
{
	//paddr_t page = pmm_alloc_page();
	heap = pmm_alloc_page_limit(0xC0000000);
	mlist_head = heap;
}

void* malloc(size_t size)
{
	struct mem_header *mlist = mlist_head;
	mlist_head = (void *)((uint32_t)mlist_head + sizeof(struct mem_header) + size);
	mlist_head->prev = mlist;
	
	mlist->size = size;
	mlist->next = mlist_head;
	
	return (uint32_t)mlist + sizeof(struct mem_header);
}

void free(void *ptr)
{
	/* not implemented */
}
