#ifndef _crtc_h_
#define _crtc_h_
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
*/

#include <stdint.h>

#define CRTC_INDEX_PORT 		0x3D4
#define CRTC_DATA_PORT 			0x3D5

#define CRTC_HTOTAL 			0x00
#define CRTC_HDISPLAY_END 		0x01
#define CRTC_HBLANKING_START 		0x02
#define CRTC_HBLANKING_END 		0x03
#define CRTC_HRETRACE_START 		0x04
#define CRTC_HRETRACE_END 		0x05
#define CRTC_VTOTAL 			0x06
#define CRTC_OVERFLOW 			0x07
#define CRTC_ROWSCAN_PRESET 		0x08
#define CRTC_MAX_SCANLINE 		0x09
#define CRTC_CURSOR_START 		0x0A
#define CRTC_CURSOR_END 		0x0B
#define CRTC_START_ADDRESS_HIGH 	0x0C
#define CRTC_START_ADDRESS_LOW 		0x0D
#define CRTC_CURSOR_LOCATION_HIGH 	0x0E
#define CRTC_CURSOR_LOCATION_LOW 	0x0F
#define CRTC_VRETRACE_START 		0x10
#define CRTC_VRETRACE_END 		0x11
#define CRTC_VDISPLAY_END 		0x12
#define CRTC_OFFSET 			0x13
#define CRTC_UNDERLINE_LOCATION 	0x14
#define CRTC_VBLANKING_START 		0x15
#define CRTC_VBLANKING_END 		0x16
#define CRTC_MODE_CONTROL 		0x17
#define CRTC_LINE_COMPARE 		0x18

void crtc_write(uint8_t index, uint8_t value);
uint8_t crtc_read(uint8_t index);

#endif
