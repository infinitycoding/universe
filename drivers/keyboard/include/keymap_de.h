#ifndef _KEYMAP_DE_H_
#define _KEYMAP_DE_H_

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
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

static uint8_t asciinormal[] = {
0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0xE1, 0xEF, '\b',
'\t', 'q', 'w',   'e', 'r', 't', 'z', 'u', 'i', 'o', 'p',0x81, '+', '\n', 0,
'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 0x94, 0x84, 0x5E, 0, '#',
'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0, '*', 0 , ' ', 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0x92, 0x97, 0x94,'-', 0x96, '5', 0x99, '+', 0x93, 0x98, 0x95, 0x90, 0x91, 0, 0, '<', 0, 0 };


static uint8_t asciishift[] = {
0, 27, '!', '"', 0x15, '$', '%', '&', '/', '(', ')', '=', '?', 0x60, '\b',
'\t', 'Q', 'W',   'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 0x9A, '*', '\n', 0,
'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 0x99, 0x8E, 0xF8, 0, 0x27,
'Y', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', 0, 0, 0, ' ', 0,
0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
'7', '8', '9', '/', '4', '5', '6', '+', '1', '2', '3', '0', 0x91, 0, 0, '>', 0, 0 };

#endif
