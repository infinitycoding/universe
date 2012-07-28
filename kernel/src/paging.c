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

#include <paging.h>
#include <pmm.h>

int INIT_PAGING(void)
{
	pd_t *pd = pd_create();
	
	pd_install(pd, PD_NOCACHE);
return 0;
	/* enable paging */
	asm volatile (
		"mov %cr0, %eax;"
		"or $0x80000000, %eax;"
		"mov %eax, %cr0"
	);
	
	return 0;
}

pd_t *pd_create()
{
	pd_t *pd = (pd_t *)pmm_alloc_page();
	pt_t *pt = NULL;
	
	int i;
	for (i = 0; i < 1024; ++i) {
		pt = (pt_t *)pmm_alloc_page();
		//pd->entries[i] = (pde_t)((uintptr_t)pt & PDE_FRAME) | PDE_PRESENT;
	}
	
	return pd;
}

void pd_destroy(pd_t *pd)
{
	pmm_mark_page_as_free((paddr_t)pd);
}

void pd_install(pd_t *pd, uint8_t flags)
{
	printf("Installing Pagedir %#X\n", (uintptr_t)pd);
	
	uint32_t descriptor = ((uintptr_t)pd & PD_FRAME) | flags;
	asm volatile ("mov %0, %%cr3" : : "r" (descriptor));
}
