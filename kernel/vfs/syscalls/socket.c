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
#include <vfs/socket.h>

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


socket_request_t *get_socket_request(struct process_state *proc, int id)
{
    return (socket_request_t*) list_get_by_int(proc->socket_requests, offsetof(socket_request_t, id), id);
}

void port_accepted(struct cpu_state **cpu)
{
    socket_request_t *req = (socket_request_t*) (*cpu)->CPU_ARG0;

    file_descriptor_t *desc = create_fd(current_thread->process);
    desc->mode = 0;
    desc->flags = O_RDWR;
    desc->permission = VFS_PERMISSION_READ | VFS_PERMISSION_WRITE;
    desc->read_descriptor->inode = req->inode;
    desc->read_descriptor->read_buffer = 0;
    desc->read_descriptor->write_buffer = 1;

    desc->write_descriptor->inode = req->inode;
    desc->write_descriptor->read_buffer = 0;
    desc->write_descriptor->write_buffer = 1;

    (*cpu)->CPU_ARG0 = desc->id;
}

void usys_connect(struct cpu_state **cpu)
{
    static int req_id_counter = 1;

    int pid = (int) (*cpu)->CPU_ARG1;
    char *port = (char *) (*cpu)->CPU_ARG2;

    struct process_state *proc = process_find(pid);

    socket_request_t *req = (socket_request_t*) malloc(sizeof(socket_request_t));
    req->pid = current_thread->process->pid;
    req->port = malloc(strlen(port)+1);
    strcpy(req->port,port);
    req->id = req_id_counter++;
    req->event_id = add_event_trigger(0, current_thread, port_accepted);

    list_push_back(proc->socket_requests, req);
    (*cpu)->CPU_ARG0 = (uint32_t) req;

    thread_suspend(current_thread);
    *cpu = (struct cpu_state *)task_schedule(*cpu);

    send_event(proc->socket_event_id);
}

void usys_readport(struct cpu_state **cpu)
{
    char *port = (char *) (*cpu)->CPU_ARG1;

    socket_request_t *request = port_fetch(current_thread->process, port);
    if(request != NULL)
    {
        (*cpu)->CPU_ARG0 = request->id;
    }
    else
    {
        current_thread->process->socket_event_id = add_event_trigger(0, current_thread, usys_readport);
        thread_suspend(current_thread);
        *cpu = (struct cpu_state *)task_schedule(*cpu);
    }
}

void usys_accept(struct cpu_state **cpu)
{
    int id = (*cpu)->CPU_ARG1;

    socket_request_t *request = get_socket_request(current_thread->process, id);
    port_accept(current_thread->process, request);

    if(request != NULL)
        (*cpu)->CPU_ARG0 = _SUCCESS;
    else
        (*cpu)->CPU_ARG0 = _FAILURE;
}


