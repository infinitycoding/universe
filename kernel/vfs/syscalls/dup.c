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

#include <sched/scheduler.h>
#include <sched/thread.h>
#include <sched/process.h>

extern struct thread_state *current_thread;

void sys_dup(struct cpu_state **cpu)
{
    int oldfd_id = (*cpu)->CPU_ARG1;

    file_descriptor_t *oldfd = get_fd(current_thread->process, oldfd_id);
    file_descriptor_t *newfd = create_fd(current_thread->process);

    newfd->permission = oldfd->permission;
    newfd->mode = oldfd->mode;
    newfd->flags = oldfd->flags;
    newfd->read_descriptor = oldfd->read_descriptor;
    newfd->write_descriptor = oldfd->write_descriptor;

    (*cpu)->CPU_ARG0 = newfd->id;
}

void sys_dup2(struct cpu_state **cpu)
{
    int oldfd_id = (*cpu)->CPU_ARG1;
    int newfd_id = (*cpu)->CPU_ARG2;

    file_descriptor_t *oldfd = get_fd(current_thread->process, oldfd_id);
    file_descriptor_t *newfd = get_fd(current_thread->process, newfd_id);

    struct list_node *node = current_thread->process->files->head->next;
    while( ! list_is_last(current_thread->process->files))
    {
        file_descriptor_t *desc = node->element;
        if(desc == newfd)
        {
            list_remove_node(node);

            newfd->permission = oldfd->permission;
            newfd->mode = oldfd->mode;
            newfd->flags = oldfd->flags;
            newfd->read_descriptor = oldfd->read_descriptor;
            newfd->write_descriptor = oldfd->write_descriptor;

            (*cpu)->CPU_ARG0 = newfd->id;
            return;
        }
        else
        {
            node = node->next;
        }
    }

    (*cpu)->CPU_ARG0 = -1;
}

