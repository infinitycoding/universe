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
  @author Thomas Haller (Universe Team) <thomas.haller@familie-haller.eu>
  @author Simon Diepold aka. Tdotu (Universe Team) <simon.diepold@infinitycoding.de>
  @author Michael Sippel (Universe Team) <micha.linuxfreak@gmail.com>
*/

#include "memory_layout.h"

section .multiboot
#define MB_MAGIC 0x1BADB002
#define MB_FLAGS 0x0
#define MB_CHECKSUM -(MB_MAGIC + MB_FLAGS)

align 4
dd MB_MAGIC
dd MB_FLAGS
dd MB_CHECKSUM

section .data
align 4096
boot_pd:
    dd 0x00000083
    times (KERNEL_PAGES - 1) dd 0
    dd 0x00000083
    times (1024 - KERNEL_PAGES - 1) dd 0

/**
 * setting up Paging and call the init function
 * @param pointer to the multiboot structures
 * @param Checksum
 * @return void
 **/
section .text

global start
start:
  mov  ecx, boot_pd - MEMORY_LAYOUT_KERNEL_START
  mov  cr3, ecx

  mov  ecx, cr4
  or   ecx, (1 << 4)
  mov  cr4, ecx
  
  mov  ecx, cr0
  or   ecx, (1 << 31)
  mov  cr0, ecx
  
  lea ecx, [higherhalf]
  jmp ecx

higherhalf:
  mov esp, stack
  add ebx, MEMORY_LAYOUT_KERNEL_START
  
  push 0
  push 0
  push eax
  push ebx
  
extern init
  call init
  jmp $

section .bss
align 4096
resb 4096
stack:
