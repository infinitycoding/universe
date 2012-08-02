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

#include <stdint.h>
#include <printf.h>
#include <multiboot.h>
#include <panic.h>

#include <pmm.h>
#include <paging.h>
#include <gdt.h>
#include <idt.h>
#include <io.h>
#include <cpuid.h>
#include <logo.h>

#include <drivers/keyboard.h>
#include <drivers/timer.h>
#include <drivers/cmos.h>
#include <drivers/video.h>


/**
* Initalize the Kernel
*
* @param mb_info The pointer to the multiboot-struct from the bootloader
* @param magic_number Multiboot-magic
*
* @return 0
*/
int init (struct multiboot_struct *mb_info, uint32_t magic_number)
{
	clear_screen();

	if (magic_number != 0x2BADB002) {
		panic("Incompatible Bootloader");
	}
	
	set_color(WHITE | BLACK << 4);
	
	INIT_PMM(mb_info);
	INIT_GDT();
	INIT_IDT();
	INIT_PAGING();
	INIT_CPUID();
	INIT_PIT(50);
	INIT_CMOS();
	INIT_KEYBOARD();
	INIT_MALLOC();
	asm volatile("sti");

	print_logo(YELLOW);
	puts("Universe wird gestartet...\n");
	uint32_t pages = pmm_count_free_pages();
	printf("%u freie Speicherseiten (%u MB)\n", pages, pages >> 8);
	print_time(get_time());
	
	int32_t *test = 0x1337;
	*test = 10;
	
	for (;;) {
		putchar(input());
	}

	return 0;
}

