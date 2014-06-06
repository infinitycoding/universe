#ifndef _CMOS_H_
#define _CMOS_H_

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
 *  @author Michael Sippel <micha.linuxfreak@gmail.com>
 */


#include <sys/types.h>

struct cmos_register
{
    uint8_t register_a;
    uint8_t register_b;
    uint8_t register_c;
    uint8_t register_d;
} cmos_register;

struct cmos_hardware_data
{
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
};

struct cmos_data
{
    struct cmos_register registers;
    struct cmos_hardware_data hardware;
};

void INIT_CMOS(void);
void get_cmos_data(struct cmos_data *cmos);

uint8_t cmos_read_byte(uint8_t offset);
void cmos_write_byte(uint8_t offset, uint8_t value);

#endif

