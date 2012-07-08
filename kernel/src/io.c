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

uint8_t inb(uint16_t port) {
	uint8_t result;
	asm volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

uint16_t inw(uint16_t port) {
	uint16_t result;
	asm volatile("inw %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

uint32_t inl(uint16_t port) {
	uint32_t result;
	asm volatile("inl %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

void outb(uint16_t port, uint8_t value) {
	asm volatile("outb %1, %0" : : "Nd" (port), "a" (value));
}

void outw(uint16_t port, uint16_t value) {
	asm volatile("outw %1, %0" : : "Nd" (port), "a" (value));
}

void outl(uint16_t port, uint32_t value) {
	asm volatile("outl %1, %0" : : "Nd" (port), "a" (value));
}
