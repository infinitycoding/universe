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
**/

#include "memory_layout.h"

[BITS 32]
FLAGS    equ 0
MAGIC    equ 0x1BADB002       ; Magicnumber
CHECKSUM equ -(MAGIC + FLAGS) ; Checksum
//zeilen equ 10


section .multiboot
align 16
MultiBootHeader:
  dd MAGIC       ; Magic number
  dd FLAGS       ; Flags
  dd CHECKSUM    ; Checksu


section	.multiboot.start exec
align 16

/**
 * setting up Paging and call the init function
 * @param pointer to the multiboot structures
 * @param Checksum
 * @return void
 **/
global start
start:

// Load PD adress to CR3
extern BOOT_PDE
  mov  ecx, BOOT_PDE - MEMORY_LAYOUT_KERNEL_START
  mov  cr3, ecx

// activate PSE
  mov  ecx, cr4
  or   ecx, (1 << 4)
  mov  cr4, ecx

// activate paging
  mov  ecx, cr0
  or   ecx, (1 << 31)
  mov  cr0, ecx

// loading stackpointer
  mov  esp, stack

// push parameters and align stack
  push 0
  push 0
  push eax
  push ebx

// call init
extern init
  call init
  jmp $


// thomas: ich würde den stack nicht hier anlegen sondern im C code
section .bss
align 4096
resb 4096
stack:
