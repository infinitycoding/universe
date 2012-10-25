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



#include <gdt.h>

static struct gdt_entry GDT[7]; //nulldesc,Datadesc0,Codedesc0,Datadesc3,Codedesc3,TSS,Callgate 6 descs +1 opt.
static struct gdtpt gdtp;

void set_GDT_entry(int entry, uint32_t base, uint32_t size, uint8_t acess, int8_t flags) {
	GDT[entry].limit_low =(uint16_t)size;
	GDT[entry].Base_low =(uint16_t)base;
	GDT[entry].Base_middle =(uint8_t)(base>>16);
	GDT[entry].Access = acess;
	GDT[entry].limit_Flags=(uint8_t) ((flags<<4)|(size>>16));
	GDT[entry].base_high =(uint8_t) (base>>24);
}
void load_gdt(uint16_t last_entry) {
	gdtp.limit = ((last_entry+1)*8)-1;
	gdtp.base = GDT;
	asm volatile("lgdt %0"::"m" (gdtp));
}
void INIT_GDT(void) {
	set_GDT_entry(0,0,0,0,0);
	set_GDT_entry(1,0,0xFFFFF,0x9A,0xC);
	set_GDT_entry(2,0,0xFFFFF,0x92,0xC);
	set_GDT_entry(3,0,0xfffff,0xFA,0xC);
	set_GDT_entry(4,0,0xfffff,0xF2,0xC);
	load_gdt(4);
	asm volatile(
		"mov $0x10, %ax;"
		"mov %ax, %ds;"
		"mov %ax, %es;"
		"mov %ax, %fs;"
		"mov %ax, %gs;"
		"mov %ax, %ss;"
		"ljmp $0x8, $.1;"
		".1:;"
	);
}
