/*
	Copyright 2012-2014 universe coding group (UCG) all rights reserved
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

#include <memory_layout.h>
#include <mm/paging.h>
#include <mm/heap.h>

#include <sched/scheduler.h>
#include <sched/thread.h>
#include <sched/process.h>

#include <clock.h>
#include <event/trigger.h>

extern struct thread_state *current_thread;
extern vfs_inode_t *root;


void sys_link(struct cpu_state **cpu)
{
    char *src_path = (char *)(*cpu)->CPU_ARG1;
    char *dest_path = (char *)(*cpu)->CPU_ARG2;

    vfs_inode_t *src_inode = vfs_lookup_path(src_path);

    // FIXME: only works in root
    vfs_inode_t *dest_parent = root; // FIXME TODO

    if(src_inode != NULL && dest_parent != NULL)
    {
        if(vfs_access(src_inode, R_OK, current_thread->process->uid, current_thread->process->gid == 0) &&
                vfs_access(dest_parent, W_OK, current_thread->process->uid, current_thread->process->gid == 0))
        {
            vfs_inode_t *dest_inode = vfs_create_inode(dest_path, src_inode->stat.st_mode | S_IFLNK, dest_parent, current_thread->process->uid, current_thread->process->gid);

            if(dest_inode != NULL)
            {
                dest_inode->buffer = (void *)dest_inode;

                (*cpu)->CPU_ARG0 = _SUCCESS;
            }
            else
            {
                (*cpu)->CPU_ARG0 = _FAILURE;
            }
        }
        else
        {
            (*cpu)->CPU_ARG0 = _NO_PERMISSION;
        }
    }
    else
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
    }
}

void sys_unlink(struct cpu_state **cpu)
{
    char *path = (char *)(*cpu)->CPU_ARG1;

    vfs_inode_t *link = vfs_lookup_path(path);
    if(link != NULL)
    {
        if(vfs_access(link, W_OK, current_thread->process->uid, current_thread->process->gid == 0))
        {
            link->buffer = NULL;
            link->type = VFS_REGULAR;
            free(link);
            (*cpu)->CPU_ARG0 = _SUCCESS;
        }
        else
        {
            (*cpu)->CPU_ARG0 = _NO_PERMISSION;
        }
    }
    else
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
    }
}

