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
*/

/**
	@author Tom Slawik <tom.slawik@gmail.com>
	@author Michael Sippel (Universe Team) <micha.linuxfreak@gmail.com>
*/

#include <paging.h>
#include <arch_paging.h>
#include <memory_layout.h>
#

vmm_context_t *current_context = NULL;

/**
 * Initalize paging
 *
 * @param void
 * @return void
 */
void INIT_PAGING(struct multiboot_struct *mb_info) {
	install_exc(0xe, page_fault_handler);
	ARCH_INIT_PAGING(mb_info);
}

/**
 * Create & Destroy
 */
void vmm_create_context(vmm_context_t *context) {
	context->arch_context = arch_vmm_create_context();
}

void vmm_destroy_context(vmm_context_t *context) {
	arch_vmm_destroy_context(context->arch_context);
}

void vmm_switch_context(vmm_context_t *context) {
	arch_update_context(context->arch_context);
	current_context = context;
	arch_switch_context(context->arch_context);
}

/**
 * Map a physical address to a virtual adress
 */
int vmm_map(vmm_context_t *context, paddr_t pframe, vaddr_t vframe, uint8_t flags) {
	return arch_map(context->arch_context, pframe, vframe, flags);
}

int vmm_unmap(vmm_context_t *context, vaddr_t frame) {
	return arch_unmap(context->arch_context, frame);
}

/**
 * Range
 */
int vmm_map_range(vmm_context_t *context, paddr_t pframe, vaddr_t vframe, int pages, uint8_t flags) {
	int p;
	for (p = 0; p < pages; ++p) {
		vmm_map(context->arch_context, pframe + PAGE_FRAME_ADDR(p), vframe + PAGE_FRAME_ADDR(p), flags);
	}
	return 0;
}

int vmm_unmap_range(vmm_context_t *context, vaddr_t frame, int pages) {
	int p;
	for(p = 0; p < pages; p++) {
		vmm_unmap(context, frame + PAGE_FRAME_ADDR(p));
	}
	return 0;
}

/**
 * Automap
 */
vaddr_t vmm_automap_kernel(vmm_context_t *context, paddr_t pframe, uint8_t flags) {
	vaddr_t vframe = arch_vaddr_find(context->arch_context, 1,
				    MEMORY_LAYOUT_RESERVED_AREA_END,
				    MEMORY_LAYOUT_KERNEL_END, flags);
	vmm_map(context, pframe, vframe, flags | VMM_PRESENT);

	return vframe;
}

vaddr_t vmm_automap_kernel_range(vmm_context_t *context, paddr_t pframe, int pages, uint8_t flags) {
	int i;
	vaddr_t vaddr_start = arch_vaddr_find(context->arch_context, pages, MEMORY_LAYOUT_RESERVED_AREA_END, MEMORY_LAYOUT_KERNEL_END, flags);
	for(i = 0; i < pages; i++) {
		paddr_t paddr = pframe + i*PAGE_SIZE;
		vaddr_t vaddr = vaddr_start + i*PAGE_SIZE;
		vmm_map(context, paddr, vaddr, flags);
	}

	return vaddr_start;
}

vaddr_t vmm_automap_user(vmm_context_t *context, paddr_t pframe, uint8_t flags) {
	vaddr_t vframe = arch_vaddr_find(context->arch_context, 1,
				    0x0, MEMORY_LAYOUT_KERNEL_START, flags);
	vmm_map(context, pframe, vframe, flags | VMM_PRESENT);

	return vframe;
}

vaddr_t vmm_automap_user_range(vmm_context_t *context, paddr_t pframe, int pages, uint8_t flags) {
	int i;
	vaddr_t vaddr_start = arch_vaddr_find(context->arch_context, pages, 0x0, MEMORY_LAYOUT_KERNEL_START, flags);
	for(i = 0; i < pages; i++) {
		paddr_t paddr = pframe + i*PAGE_SIZE;
		vaddr_t vaddr = vaddr_start + i*PAGE_SIZE;
		vmm_map(context, paddr, vaddr, flags);
	}

	return vaddr_start;
}

