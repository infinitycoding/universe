#ifndef _BLOCK_BUFFER_H_
#define _BLOCK_BUFFER_H_

/*
     Copyright 2012-2015 Infinitycoding all rights reserved
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
 *  @author Michael Sippel <micha@infinitycoding.de>
 */
#include <stdint.h>
#include <list.h>

typedef struct block_buffer_info
{
    list_t *blocks;
    size_t block_size;
    unsigned int block_counter;
} block_buffer_info_t;

typedef struct buffer_block
{
    unsigned int id;
    uint8_t *base;
} buffer_block_t;

block_buffer_info_t *block_buffer_create(size_t block_size);
buffer_block_t *buffer_block_get(block_buffer_info_t *info, unsigned int id);
buffer_block_t *buffer_block_create(block_buffer_info_t *info, unsigned int id);
buffer_block_t *buffer_block_remove(block_buffer_info_t *info, unsigned int id);
int block_buffer_write(block_buffer_info_t *info, uintptr_t offset, uint8_t *data, size_t bytes);
int block_buffer_read (block_buffer_info_t *info, uintptr_t offset, uint8_t *data, size_t bytes);

#endif

