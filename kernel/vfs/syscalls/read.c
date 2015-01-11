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

void sys_read(struct cpu_state **cpu)
{
    int fd = (*cpu)->CPU_ARG1;
    void *buf = (void*) (*cpu)->CPU_ARG2;
    size_t len = (*cpu)->CPU_ARG3;

    struct fd *desc = get_fd(current_thread->process, fd);
    if(desc != NULL)
    {
        if(desc->permission & VFS_PERMISSION_READ && desc->read_inode != NULL)
        {
            vfs_inode_t *inode = desc->read_inode;
            vfs_inode_t *real = inode;
            GET_INODE(real);
            if(S_ISDIR(real->stat))
            {
                (*cpu)->CPU_ARG0 = _NO_PERMISSION;
                return;
            }
            block_buffer_info_t *info = real->read_buffer;
            if(info == NULL)
            {
                (*cpu)->CPU_ARG0 = _FAILURE;
                return;
            }

            int ret = vfs_read(inode, desc->read_pos, buf, len);

            if(ret == len)
            {
                desc->read_pos += len;
                if(real->type != VFS_PIPE)
                {
                    desc->write_pos += len;
                }

                (*cpu)->CPU_ARG0 = len;
            }
            else
            {
                if(real->type == VFS_PIPE)
                {
                    add_trigger(WAIT_EVENT, inode->event_id, 0, current_thread, sys_read);
                    thread_suspend(current_thread);
                    *cpu = (struct cpu_state *)task_schedule(*cpu);
                }
                else
                {
                    (*cpu)->CPU_ARG0 = _FAILURE;
                }
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

