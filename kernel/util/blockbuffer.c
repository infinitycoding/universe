/*
	Copyright 2012-2015 universe coding group (UCG) all rights reserved
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
	@author Michael Sippel <micha@infinitycoding.com>
*/
#include <list.h>
#include <stddef.h>
#include <stdint.h>
#include <mm/heap.h>

#include <blockbuffer.h>

/**
 * Create a block buffer object
 *
 * @param block_size Size of one block
 * @return pointer to object
 */
block_buffer_info_t *blockbuffer_create(size_t block_size)
{
    block_buffer_info_t *buffer = (block_buffer_info_t*) malloc(sizeof(block_buffer_info_t));

    buffer->blocks = list_create();
    buffer->block_size = block_size;

    return buffer;
}

/**
 * Search a block in a buffer by ID
 *
 * @param info	the buffer object
 * @param id	identifier of searched object
 *
 * @return pointer to block object
 */
buffer_block_t *buffer_block_get(block_buffer_info_t *info, unsigned int id)
{
    return (buffer_block_t*) list_get_by_int(info->blocks, offsetof(buffer_block_t, id), id);
}

/**
 * Create a block in a buffer, if it doesn't exists
 *
 * @param info the buffer object
 * @return pointer to the block
 */
buffer_block_t *buffer_block_create(block_buffer_info_t *info, unsigned int id)
{
    buffer_block_t *block = buffer_block_get(info, id);

    if(block == NULL)
    {
        block = malloc(sizeof(buffer_block_t));

        block->base = malloc(info->block_size);
        block->id = id;
        list_push_back(info->blocks, block);
    }

    return block;
}

/**
 * Write in a blockbuffer
 *
 * @param buffer buffer info
 * @param offset position in the buffer to write
 * @param data buffer with data to write
 * @param bytes number of bytes to write
 *
 * @return number of written bytes
 */
int block_buffer_write(block_buffer_info_t *info, uintptr_t offset, uint8_t *data, size_t bytes)
{
    // calculate block indices
    int block_id  = offset / info->block_size;
    int block_off = offset % info->block_size;

    // search first block
    buffer_block_t *block = buffer_block_create(info, block_id);

    // go through all bytes...
    int i;
    for(i = 0; i < bytes; i++)
    {
        // if the block ends, go to the next
        if(block_off >= info->block_size)
        {
            block = buffer_block_create(info, ++block_id);
            block_off = 0;
        }

        // copy data
        block->base[block_off++] = data[i];
    }

    return i;
}

/**
 * Read from a blockbuffer
 *
 * @param buffer buffer info
 * @param offset position in the buffer to read
 * @param data buffer to store read data
 * @param bytes number of bytes to read
 *
 * @return number of read bytes
 */
int block_buffer_read (block_buffer_info_t *info, uintptr_t offset, uint8_t *data, size_t bytes)
{
    // calculate block indices
    int block_id  = offset / info->block_size;
    int block_off = offset % info->block_size;

    // search first block
    buffer_block_t *block = buffer_block_get(info, block_id);

    // go through all bytes...
    int i;
    for(i = 0; i < bytes; i++)
    {
        // if the block ends, go to the next
        if(block_off >= info->block_size)
        {
            block = buffer_block_get(info, ++block_id);
            block_off = 0;
        }

        if(block == NULL)
            break;

        // copy data
        data[i] = block->base[block_off++];
    }

    return i;
}

