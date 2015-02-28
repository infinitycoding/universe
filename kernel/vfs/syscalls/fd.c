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

file_descriptor_t *get_fd(struct process_state *process, int fd)
{
    return (file_descriptor_t*) list_get_by_int(process->files, offsetof(file_descriptor_t, id), fd);
}

file_descriptor_t *create_fd(struct process_state *process)
{
    file_descriptor_t *desc = malloc(sizeof(file_descriptor_t ));

    memset(desc, 0, sizeof(file_descriptor_t ));
    desc->id = list_length(process->files);

    desc->read_descriptor = vfs_create_inode_descriptor(NULL);
    desc->write_descriptor = vfs_create_inode_descriptor(NULL);

    list_push_back(process->files, desc);

    return desc;
}

void sys_open(struct cpu_state **cpu)
{
    char *path = (char *) (*cpu)->CPU_ARG1;
    int oflags = (*cpu)->CPU_ARG2;
    mode_t mode = (*cpu)->CPU_ARG3;
    vfs_inode_t *inode = vfs_lookup_path(path);

    if(inode == NULL)
    {
        if(oflags & O_CREAT)
        {
            // create inode
            inode = vfs_create_path(path, mode, current_thread->process->uid, current_thread->process->gid);
            if(inode == NULL)
            {
                (*cpu)->CPU_ARG0 = _NO_PERMISSION;
                return;
            }
        }
        else
        {
            (*cpu)->CPU_ARG0 = _FAILURE;
            return;
        }
    }
    else
    {
        if(oflags & O_EXCL)
        {
            (*cpu)->CPU_ARG0 = _FAILURE;
            return;
        }
    }

    if(oflags & O_TRUNC)
    {
        if(vfs_access(inode, W_OK, current_thread->process->uid, current_thread->process->gid) == 0)
        {
            // TODO: clear file
        }
        else
        {
            (*cpu)->CPU_ARG0 = _NO_PERMISSION;
            return;
        }
    }

    file_descriptor_t *desc = create_fd(current_thread->process);
    desc->mode = mode;
    desc->flags = oflags;

    if(oflags & O_APPEND)
    {
        desc->read_descriptor->position = inode->length;
        desc->write_descriptor->position = inode->length;
    }

    if( (oflags & O_RDONLY || oflags & O_RDWR) && vfs_access(inode, R_OK, current_thread->process->uid, current_thread->process->gid) == 0)
    {
        desc->permission |= VFS_PERMISSION_READ;
        desc->read_descriptor->inode = inode;
    }

    if( (oflags & O_WRONLY || oflags & O_RDWR) && vfs_access(inode, W_OK, current_thread->process->uid, current_thread->process->gid) == 0)
    {
        desc->permission |= VFS_PERMISSION_WRITE;
        desc->write_descriptor->inode = inode;
    }

    (*cpu)->CPU_ARG0 = desc->id;
}

void sys_close(struct cpu_state **cpu)
{
    int fd = (*cpu)->CPU_ARG1;

    struct list_node *node = current_thread->process->files->head->next;
    int i;
    for(i = 0; i < list_length(current_thread->process->files); i++)
    {
        file_descriptor_t *desc = node->element;
        if(desc->id == fd)
        {
            list_remove_node(node);
            (*cpu)->CPU_ARG0 = _SUCCESS;
            return;
        }
        else
        {
            node = node->next;
        }
    }

    (*cpu)->CPU_ARG0 = _FAILURE;
}

