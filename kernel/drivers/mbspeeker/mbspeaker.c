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

#include <io.h>

void sound_on() {
	outb(0x61, inb(0x61) | 3);
}

void sound_off() {
	outb(0x61, inb(0x61) & ~3);
}

void sound(uint32_t frequency, uint32_t time) {
	int i;
	uint16_t tmp;
	tmp = 1193180 / frequency;
	outb(0x43, 0xB6);
	outb(0x42, tmp & 0xFF);
	outb(0x42, tmp >> 8);
	sound_on();
	for (i = 0; i < time * 10000; i++);
	sound_off();
}
