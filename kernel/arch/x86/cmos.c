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
 *  @file /drivers/cmos/cmos.c
 *  @brief Kernel driver for x86 computer CMOS RAM [BIOS DATA]
 *  @author Michael Sippel <michamimosa@gmail.com>
 */

#include <io.h>
#include <arch/cmos.h>

/**
 * @brief Reads a byte from CMOS.
 * @param offset Offset in the CMOS
 * @return Read value from CMOS
 */
uint8_t cmos_read_byte(uint8_t offset)
{
    uint8_t tmp = inb(CMOS_ADDRESS_PORT);
    outb(CMOS_ADDRESS_PORT, (tmp & 0x80) | (offset & 0x7F));
    return inb(CMOS_DATA_PORT);
}

/**
 * @brief Write a byte to CMOS
 * @param offset Offset in the CMOS
 * @param value Value which is written into the CMOS
 * @return void
 */
void cmos_write_byte(uint8_t offset, uint8_t value)
{
    uint8_t tmp = inb(CMOS_ADDRESS_PORT);
    outb(CMOS_ADDRESS_PORT, (tmp & 0x80) | (offset & 0x7F));
    outb(CMOS_DATA_PORT, value);
}

