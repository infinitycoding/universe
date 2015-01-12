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

extern struct thread_state *current_thread;
extern vfs_inode_t *root;

/**
 * Dissolve a path to an inode
 *
 * @param path
 * @return inode
 */
vfs_inode_t *vfs_lookup_path(char *path)
{
    vfs_inode_t *parent = root;

    if(path[0] != '/')
    {
        parent = current_thread->process->cwd;
    }
    else
    {
        path++;
    }

    if(path[0] == '\0')
        return parent;

    int len = strlen(path);
    if(path[len-1] == '/')
    {
        path[len-1] = '\0';
    }

    char delimiter[] = "/";
    char *str = (char*) strtok(path, delimiter);
    while(str != NULL)
    {
        vfs_inode_t *real = parent;
        GET_INODE(real);

        block_buffer_info_t *info = real->read_buffer;
        iterator_t it = iterator_create(info->blocks);

        int found = 0;
        while(! list_is_last(&it) && ! list_is_empty(&it))
        {
            buffer_block_t *block = it.current->element;
            vfs_dentry_t *dentry = (vfs_dentry_t*) block->base;

            if(strcmp(str, dentry->inode->name) == 0)
            {
                parent = dentry->inode;
                found = 1;
                break;
            }

            list_next(&it);
        }

        if(!found)
        {
            return NULL;
        }
        else
        {
            str = strtok(NULL, delimiter);
        }
    }

    return parent;
}

/**
 * Dissolve a path to an inode and create all missing nodes
 *
 * @param path
 * @param mode
 * @param uid
 * @param gid
 * @return inode
 */
vfs_inode_t *vfs_create_path(char *path, mode_t mode, uid_t uid, gid_t gid)
{
    vfs_inode_t *parent = root;

    if(path[0] != '/')
    {
        parent = current_thread->process->cwd;
    }
    else
    {
        path++;
    }

    if(path[0] == '\0')
        return parent;

    int len = strlen(path);
    if(path[len-1] == '/')
    {
        path[len-1] = '\0';
    }

    char delimiter[] = "/";
    char *str = (char*) strtok(path, delimiter);
    while(str != NULL)
    {
        vfs_inode_t *real = parent;
        GET_INODE(real);

        block_buffer_info_t *info = real->write_buffer;
        iterator_t it = iterator_create(info->blocks);

        int found = 0;
        while(! list_is_last(&it) && ! list_is_empty(&it))
        {
            buffer_block_t *block = it.current->element;
            vfs_dentry_t *dentry = (vfs_dentry_t*) block->base;

            if(strcmp(str, dentry->inode->name) == 0)
            {
                parent = dentry->inode;
                found = 1;
                break;
            }

            list_next(&it);
        }

        char *new_str = strtok(NULL, delimiter);
        if(!found)
        {
            int n_mode;
            if(new_str != NULL)
                n_mode = mode & (S_IRWXU | S_IRWXG | S_IRWXO) | S_IFDIR;
            else
                n_mode = mode;

            if(vfs_access(parent, W_OK, uid, gid) == 0)
            {
                parent = vfs_create_inode(str, n_mode, parent, uid, gid);
            }
            else
            {
                return NULL;
            }
        }
        str = new_str;
    }

    return parent;
}

