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

void sys_mknod(struct cpu_state **cpu)
{
    char *path = (char *) (*cpu)->CPU_ARG1;
    int mode =(int) (*cpu)->CPU_ARG2;
    //int dev = (int)(*cpu)->CPU_ARG3; // unused

    vfs_inode_t *inode = vfs_create_path(path, mode, current_thread->process->uid, current_thread->process->gid);

    if(inode != NULL)
    {
        if(S_ISFIFO(inode->stat))
        {
            //inode->buffer->event_id = get_new_event_ID();
            //inode->buffer->handlers = list_create();
            inode->type = VFS_PIPE;
        }

        (*cpu)->CPU_ARG0 = _SUCCESS;
    }
    else
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
    }
}

void sys_create(struct cpu_state **cpu)
{
    char *name = (char *)(*cpu)->CPU_ARG1;
    int mode = (*cpu)->CPU_ARG2;

    vfs_inode_t *inode = vfs_create_path(name, mode, current_thread->process->uid, current_thread->process->gid);

    if(inode != NULL)
    {
        struct fd *desc = create_fd(current_thread->process);
        desc->mode = mode;
        desc->flags = O_RDWR;
        desc->permission = VFS_PERMISSION_READ | VFS_PERMISSION_WRITE;
        desc->read_inode = inode;
        desc->write_inode = inode;

        (*cpu)->CPU_ARG0 = desc->id;
    }
    else
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
    }
}

