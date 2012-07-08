#ifndef	_limits_h_
#define	_limits_h_
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

	#define INT8_MIN -128
	#define INT8_MAX 127
	#define UINT8_MAX 255

	#define INT16_MIN -32768
	#define INT16_MAX 32767
	#define UINT16_MAX 65535

	#define INT32_MIN ((int32_t) -0x80000000)
	#define INT32_MAX 0x7FFFFFFF
	#define UINT32_MAX 0xFFFFFFFF

	#define INT64_MIN ((int64_t) -0x8000000000000000LL)
	#define INT64_MAX 0x7FFFFFFFFFFFFFFFLL
	#define UINT64_MAX ((uint64_t) -1ULL)

	#define INTPTR_MIN INT32_MIN
	#define INTPTR_MAX INT32_MAX
	#define UINTPTR_MAX UINT32_MAX

	#define INTMAX_MIN INT64_MIN
	#define INTMAX_MAX INT64_MAX
	#define UINTMAX_MAX UINT64_MAX
	
#endif