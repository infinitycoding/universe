/*
	Copyright 2013 universe coding group (UCG) all rights reserved
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

**/

/**
	@author Michael Sippel <micha.linuxfreak@gmail.com>
 */

#include <elf.h>
#include <paging.h>
#include <pmm.h>
#include <memory_layout.h>
#include <process.h>
#include <thread.h>
#include <printf.h>
#include <string.h>

struct process_state *load_elf(void *image) {
	struct elf_header *header = image;
	struct elf_program_header *ph;

	if(header->ident[EI_MAG0] == ELF_MAG0 &&
	   header->ident[EI_MAG1] == ELF_MAG1 &&
	   header->ident[EI_MAG2] == ELF_MAG2 &&
	   header->ident[EI_MAG3] == ELF_MAG3)
	{
	} else {
		printf("Invalid ELF-Magic!\n");
		return NULL;
	}

	int i,j;
	ph = (struct elf_program_header*) (image + header->ph_offset);

	struct process_state *proc = process_create("", "", PROCESS_ACTIVE, NULL, NULL, NULL, NULL);
	struct thread_state *new_thread = thread_create(proc, 3, header->entry, NULL, 0, NULL, NULL, NULL);

	for(i = 0; i < header->ph_entry_count; i++, ph++) {
		if(ph->type == EPT_LOAD) {
			int pages = NUM_PAGES(ph->mem_size);
			uintptr_t dest_start = (uintptr_t) arch_vaddr_find((arch_vmm_context_t *)current_context, pages,
						MEMORY_LAYOUT_KERNEL_START, MEMORY_LAYOUT_KERNEL_END, VMM_WRITABLE);

			for(j = 0; j < pages; j++) {
				paddr_t paddr = (uintptr_t) pmm_alloc_page();
				vaddr_t vaddr = (uintptr_t) ph->virt_addr + j*PAGE_SIZE;
				uintptr_t src = (uintptr_t) image + ph->offset + j*PAGE_SIZE;
				uintptr_t dest = (uintptr_t) dest_start + j*PAGE_SIZE;

				vmm_map(&new_thread->context, paddr, vaddr, VMM_WRITABLE | VMM_USER);
				vmm_map(current_context, paddr, dest, VMM_WRITABLE);

        			memcpy((void*) dest, (void*) src, PAGE_SIZE);
      			}
      			memset((void*)dest_start + ph->file_size, 0, ph->mem_size - ph->file_size);

      			vmm_unmap_range(current_context, dest_start, pages);
    		}
  	}

	return proc;
}

