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

#include <vfs/fd.h>
#include <vfs/vfs.h>
#include <mm/heap.h>

vfs_inode_descriptor_t* vfs_create_inode_descriptor(vfs_inode_t *inode)
{
    vfs_inode_descriptor_t *desc = malloc(sizeof(vfs_inode_descriptor_t));

    desc->inode = inode;
    desc->position = 0;

    return desc;
}

size_t vfs_write_descriptor(vfs_inode_descriptor_t *desc, void *buffer, size_t length)
{
    size_t n = vfs_write(desc->inode, desc->position, buffer, length);
    desc->position += n;

    return n;
}

size_t vfs_read_descriptor(vfs_inode_descriptor_t *desc, void *buffer, size_t length)
{
    size_t n = vfs_read(desc->inode, desc->position, buffer, length);
    desc->position += n;

    return n;
}

