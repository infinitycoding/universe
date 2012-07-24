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

    Das Universe Kernel ist Freie Software: Sie k�nnen es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder jeder sp�teren
    ver�ffentlichten Version, weiterverbreiten und/oder modifizieren.

    Das Universe Kernel wird in der Hoffnung, dass es n�tzlich sein wird, aber
    Universe Kernel wird in der Hoffnung, dass es n�tzlich sein wird, aber
    OHNE JEDE GEW�HELEISTUNG, bereitgestellt; sogar ohne die implizite
    Gew�hrleistung der MARKTF�HIGKEIT oder EIGNUNG F�R EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License f�r weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <multiboot.h>
#include <panic.h>
#include <pmm.h>
#include <vmm.h>
#include <gdt.h>
#include <idt.h>
#include <io.h>
#include <printf.h>
#include <driver/keyboard.h>
#include <driver/timer.h>
#include <driver/cmos.h>
#include <cpuid.h>

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
		panic("incompatible bootloader");
	}
	
	INIT_PMM(mb_info);
	INIT_VMM();
	INIT_GDT();
	INIT_IDT();
	INIT_CPUID();
	INIT_PIT(50);
	INIT_CMOS();
	//asm volatile("sti");

	INIT_KEYBOARD();

	time_t *time = get_time();
	char *day_string;
	switch (time->week_day) {
		case 0: day_string = "Sonntag";		break;
		case 1: day_string = "Montag";		break;
		case 2: day_string = "Dienstag";	break;
		case 3: day_string = "Mittwoch";	break;
		case 4: day_string = "Donnerstag";	break;
		case 5: day_string = "Freitag";		break;
		case 6: day_string = "Samstag";		break;
	}
	
	printf("Welcome to Universe!\n\n");
	
	printf("System Date: %02d/%02d/%02d (%s)\n", time->day_in_month, time->month, time->year, day_string);
	printf("System Time: %02d:%02d:%02d\n\n", time->hour, time->minute, time->second);
	
	CPU_info();
	
	for(;;);
	
	return 0;
}
