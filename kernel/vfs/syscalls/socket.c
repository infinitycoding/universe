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


socket_request_t *get_socket_request(struct process_state *proc, int id)
{
    return (socket_request_t*) list_get_by_int(proc->socket_requests, offsetof(socket_request_t, id), id);
}

void port_accepted(struct cpu_state **cpu)
{
    socket_request_t *req = (socket_request_t*) (*cpu)->CPU_ARG0;

    struct fd *desc = create_fd(current_thread->process);
    desc->mode = 0;
    desc->flags = O_RDWR;
    desc->permission = VFS_PERMISSION_READ | VFS_PERMISSION_WRITE;
    desc->read_inode = req->inodes[0];
    desc->write_inode = req->inodes[1];

    (*cpu)->CPU_ARG0 = desc->id;
}

void usys_connect(struct cpu_state **cpu)
{
    static int req_id_counter = 1;

    int pid = (int) (*cpu)->CPU_ARG1;
    int port = (int) (*cpu)->CPU_ARG2;

    struct process_state *proc = process_find(pid);

    socket_request_t *req = (socket_request_t*) malloc(sizeof(socket_request_t));
    req->pid = current_thread->process->pid;
    req->port = port;
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
    int port = (int) (*cpu)->CPU_ARG1;
    if(! list_is_empty(current_thread->process->socket_requests))
    {
        socket_request_t *req = (socket_request_t*) list_get_by_int(current_thread->process->socket_requests, offsetof(socket_request_t, port), port);
        (*cpu)->CPU_ARG0 = req->id;
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

    struct list_node *node = list_get_node_by_int(current_thread->process->socket_requests, offsetof(socket_request_t, id), id);
    if(node == NULL)
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
        return;
    }

    socket_request_t *req = (socket_request_t*) node->element;
    list_remove_node(node);

    char pstr[16];
    sprintf(pstr, "%d", req->port);

    vfs_dentry_t *dentry = vfs_get_dir_entry(current_thread->process->socket_inode, pstr);

    if(dentry != NULL && dentry->inode != NULL)
    {
        char rstr[64], wstr[64];
        sprintf(rstr, "%d.in", req->pid);
        sprintf(wstr, "%d.out", req->pid);

        vfs_inode_t *r_in = vfs_create_inode(rstr, 0, dentry->inode, 0, 0);
        vfs_inode_t *w_in = vfs_create_inode(wstr, 0, dentry->inode, 0, 0);
        r_in->buffer->event_id = get_new_event_ID();
        r_in->buffer->handlers = list_create();
        r_in->type = VFS_PIPE;
        w_in->buffer->event_id = get_new_event_ID();
        w_in->buffer->handlers = list_create();
        w_in->type = VFS_PIPE;

        struct fd *desc = create_fd(current_thread->process);
        desc->mode = 0;
        desc->flags = O_RDWR;
        desc->permission = VFS_PERMISSION_READ | VFS_PERMISSION_WRITE;
        desc->read_inode = r_in;
        desc->write_inode = w_in;

        (*cpu)->CPU_ARG0 = desc->id;

        req->inodes[0] = w_in;
        req->inodes[1] = r_in;

        send_event(req->event_id);
    }
    else
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
    }
}


