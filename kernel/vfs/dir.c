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


#include <vfs/vfs.h>
#include <vfs/fd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include <printf.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include <mm/heap.h>
#include <mm/paging.h>
#include <memory_layout.h>

#include <sched/thread.h>
#include <sched/scheduler.h>

#include <clock.h>
#include <event/trigger.h>


/**
 * Create a new directory entry
 *
 * @param entry_inode the node that will be entried
 * @return the new directory entry
 */
vfs_dentry_t* vfs_create_dir_entry(vfs_inode_t *entry_inode)
{
    vfs_dentry_t *dir_ent = malloc(sizeof(vfs_dentry_t));

    dir_ent->ino = entry_inode->stat.st_ino;
    dir_ent->inode = entry_inode;

    return dir_ent;
}

void vfs_add_dir_entry(vfs_inode_t *dir, vfs_dentry_t *entry)
{
    if(! S_ISDIR(dir->stat))
        return;

    if(dir->write_buffer == NULL)
    {
        printf("leerer puffer\n");
        return;
    }

    buffer_block_t *block = buffer_block_create(dir->write_buffer, dir->write_buffer->block_counter);
    block->base = entry;
}

void vfs_remove_dir_entry(vfs_inode_t *dir, vfs_inode_t *inode)
{
    if(! S_ISDIR(dir->stat))
        return;

    if(dir->write_buffer == NULL)
    {
        printf("leerer puffer\n");
        return;
    }

    block_buffer_info_t *info = inode->write_buffer;

    iterator_t it = iterator_create(info->blocks);
    list_set_first(&it);

    while(! list_is_last(&it) && ! list_is_empty(&it))
    {
        buffer_block_t *block = (buffer_block_t*) it.current->element;
        vfs_dentry_t *dentry = (vfs_dentry_t*) block->base;

        if(dentry->inode == inode)
        {
            list_remove(&it);
            break;
        }

        list_next(&it);
    }
}

vfs_dentry_t *vfs_get_dir_entry(vfs_inode_t *ino, const char *name)
{
    GET_INODE(ino);
    if(S_ISDIR(ino->stat))
    {
        block_buffer_info_t *info = ino->read_buffer;

        iterator_t it = iterator_create(info->blocks);
        list_set_first(&it);

        while(! list_is_last(&it) && ! list_is_empty(&it))
        {
            buffer_block_t *block = (buffer_block_t*) it.current->element;
            vfs_dentry_t *dentry = (vfs_dentry_t*) block->base;

            if(strcmp(name, dentry->inode->name) == 0)
            {
                return dentry;
            }

            list_next(&it);
        }
    }

    return NULL;
}

