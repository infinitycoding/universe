#ifndef _cmos_h_
#define _cmos_h_
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

/**
	@author Michael Sippel <micha.linuxfreak@gmail.com>
*/
	#include <stdint.h>
	#include <drivers/timer.h>

	typedef struct cmos_register  {
		uint8_t register_a;
		uint8_t register_b;
		uint8_t register_c;
		uint8_t register_d;
	} cmos_register_t;

	typedef struct cmos_hardware_data {
		uint8_t post_diagnostig_status_byte;
		uint8_t shutdown_status_byte;
		uint8_t floppy_disk_type;
//		uint8_t reserved0;
		uint8_t hd_type;
//		uint8_t reserved1;
		uint8_t device_byte;
		
		uint8_t basememory_size_low;
		uint8_t basememory_size_high;
		uint8_t expandablememory_size_low;
		uint8_t expandablememory_size_high;
		uint8_t extension_byte_hd1;
		uint8_t extension_byte_hd2;

//		uint16_t reserved2;
//		uint8_t reserved3 : 2;
		
		uint8_t cmos_magic_low;
		uint8_t cmos_magic_high;

		uint8_t extendedmemory_low;
		uint8_t extendedmenory_high;

//		uint16_t reserved3 : 13;
	} cmos_hardware_data_t;

	typedef struct cmos_data {
		time_t *time;
		cmos_register_t registers;
		cmos_hardware_data_t hardware;
	} cmos_data_t;


	void INIT_CMOS(void);
	cmos_data_t* get_cmos_data(void);

	uint8_t cmos_read_byte(uint8_t offset);
	void cmos_write_byte(uint8_t offset,uint8_t value);

#endif

