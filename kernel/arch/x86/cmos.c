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
#include <cmos.h>

/**
 * @brief Reads the Values from Complementary Metal Oxide Semiconductor.
 * @param buffer
 * @return void
 */
void get_cmos_data(struct cmos_data *cmos)
{
    cmos->registers.register_a = cmos_read_byte(CMOS_TIMER_FREQ_REG);
    cmos->registers.register_b = cmos_read_byte(CMOS_TIMER_SETTING);
    cmos->registers.register_c = cmos_read_byte(CMOS_SRC_REG);
    cmos->registers.register_d = cmos_read_byte(CMOS_VALIDITY_REG);

    cmos->hardware.post_diagnostig_status_byte = cmos_read_byte(0x0E);
    cmos->hardware.shutdown_status_byte =        cmos_read_byte(0x0F);
    cmos->hardware.floppy_disk_type =            cmos_read_byte(0x10);
    cmos->hardware.hd_type =                     cmos_read_byte(0x12);
    cmos->hardware.device_byte =                 cmos_read_byte(0x14);

    cmos->hardware.basememory_size_low =         cmos_read_byte(0x15);
    cmos->hardware.basememory_size_high =        cmos_read_byte(0x16);
    cmos->hardware.expandablememory_size_low =   cmos_read_byte(0x17);
    cmos->hardware.expandablememory_size_high =  cmos_read_byte(0x18);
    cmos->hardware.extension_byte_hd1 =          cmos_read_byte(0x19);
    cmos->hardware.extension_byte_hd2 =          cmos_read_byte(0x1A);

    cmos->hardware.cmos_magic_low =              cmos_read_byte(0x2E);
    cmos->hardware.cmos_magic_high =             cmos_read_byte(0x2F);

    cmos->hardware.extendedmemory_low =          cmos_read_byte(0x30);
    cmos->hardware.extendedmenory_high =         cmos_read_byte(0x31);
}

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

