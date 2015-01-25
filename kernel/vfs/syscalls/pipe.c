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


void sys_pipe(struct cpu_state **cpu)
{
    int *id = (int *) (*cpu)->CPU_ARG1;

    vfs_inode_t *inode = vfs_create_pipe(current_thread->process->uid, current_thread->process->gid);

    // create read channel
    struct fd *desc0 = create_fd(current_thread->process);
    id[0] = desc0->id;
    desc0->mode = O_APPEND;
    desc0->flags = O_RDONLY;
    desc0->permission =  VFS_PERMISSION_READ;
    desc0->read_inode = inode;

    // create write channel
    struct fd *desc1 = create_fd(current_thread->process);
    id[1] = desc1->id;
    desc1->mode = O_APPEND;
    desc1->flags = O_WRONLY;
    desc1->permission = VFS_PERMISSION_WRITE;
    desc1->write_inode = inode;
    list_push_back(current_thread->process->files, desc1);

    //printf("kernel: piieeepe %d %d\n", desc0->id, desc1->id);

    if(desc0 != NULL && desc1 != NULL && inode != NULL)
    {
        (*cpu)->CPU_ARG0 = _SUCCESS;
    }
    else
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
    }
}


void set_pipe_trigger(struct cpu_state **cpu)
{
    int fd = (*cpu)->CPU_ARG1;
    struct fd *desc = get_fd(current_thread->process, fd);
    vfs_inode_t *inode = desc->read_inode;

    if(desc->permission & VFS_PERMISSION_READ)
    {
        //block_buffer_info_t *pipe = inode->read_buffer;

        vfs_pipe_trigger_t *trigger = malloc(sizeof(vfs_pipe_trigger_t));
        trigger->eip = (*cpu)->CPU_ARG2;
        trigger->argc = (*cpu)->CPU_ARG3;
        trigger->argv = (char**)(*cpu)->CPU_ARG4;
        trigger->environ = (char**)(*cpu)->CPU_ARG5;
        list_push_back(inode->handlers, (void*) trigger);

        (*cpu)->CPU_ARG0 = _SUCCESS;
    }
    else
    {
        (*cpu)->CPU_ARG0 = _NO_PERMISSION;
    }
}

