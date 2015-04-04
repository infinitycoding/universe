/*
     Copyright 2012-2014 Infinitycoding all rights reserved
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
 *  @file /arch/x86/boot.asm
 *  @brief Kernel Startcode For I386 based Processors with MMU.
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 *  @author Michael Sippel <micha@infinitycoding.de>
 */

#define MEMORY_LAYOUT_KERNEL_START 0xC0000000
#define KERNEL_PAGES (MEMORY_LAYOUT_KERNEL_START >> 22)

section .multiboot
#define MB_MAGIC 0x1BADB002
#define MB_FLAGS 0x0
#define MB_CHECKSUM -(MB_MAGIC + MB_FLAGS)

/// brief Multiboot header
align 4
dd MB_MAGIC     /// @var Multiboot magic number (0x1BADB002)
dd MB_FLAGS     /// @var Multiboot flags (0x0)
dd MB_CHECKSUM  /// @var Multiboot checksum

section .data
align 4096
boot_pd: /// @var Page Directory for higher half mapping of the kernel
    dd 0x00000083
    times (KERNEL_PAGES - 1) dd 0
    dd 0x00000083
    times (1024 - KERNEL_PAGES - 1) dd 0


section .text

/**
 *  @brief Setting up Paging and call the Kernel init function.
 *  @param pointer to the multiboot structures
 *  @param Checksum
 *  @return void
 */

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
  push 0
  push 0
  push eax
  push ebx
  
extern init
  call init
  jmp $

section .bss
align 4096
resb 4096 /// @var 4096 byte startup stack
stack:
