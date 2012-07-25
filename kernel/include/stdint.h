#ifndef	_stdint_h_
#define	_stdint_h_
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

#define TRUE			1
#define true			1
#define FALSE			0
#define false			0
#define NULL 			0

typedef unsigned char     	uint8_t;
typedef unsigned short    	uint16_t;
typedef unsigned int      	uint32_t;
typedef unsigned long long 	uint64_t;

typedef signed char		int8_t;
typedef signed short 		int16_t;
typedef signed int 		int32_t;
typedef signed long long 	int64_t;

typedef unsigned int 		uintptr_t;
typedef long int 		intptr_t;

typedef int64_t 		intmax_t;
typedef uint64_t 		uintmax_t;

typedef unsigned int 		size_t;

typedef unsigned int 		paddr_t;
typedef unsigned int 		vaddr_t;

typedef int 			bool;
typedef int 			boolean;

//textcolors
#define BLACK			0x0
#define BLUE			0x1
#define GREEN			0x2
#define CYAN			0x3
#define RED 			0x4
#define MAGENTA			0x5
#define BROWN			0x6
#define LIGHT_GRAY		0x7
#define DARK_GRAY		0x8
#define LIGHT_BLUE		0x9
#define LIGHT_GREEN		0xA
#define LIGHT_CYAN		0xB
#define LIGHT_RED		0xC
#define LIGHT_MAGENTA	0xD
#define YELLOW			0xE
#define WHITE			0xF


#endif
