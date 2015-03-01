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

int port_fetch(char *port)
{
    if(! list_is_empty(current_thread->process->socket_requests))
    {
        iterator_t i = iterator_create(current_thread->process->socket_requests);
        socket_request_t *req;
        while(!list_is_last(&i))
        {
            req = (socket_request_t*) list_get_current(&i);
            if(strcmp(req->port,port)== 0)
            {
                return req->id;
            }
            list_next(&i);
        }
    }
    return 0;
}

file_descriptor_t *port_accept(int id)
{
    struct list_node *node = list_get_node_by_int(current_thread->process->socket_requests, offsetof(socket_request_t, id), id);
    if(node == NULL)
    {
        return NULL;
    }

    socket_request_t *req = (socket_request_t*) node->element;
    list_remove_node(node);



    vfs_dentry_t *dentry = vfs_get_dir_entry(current_thread->process->socket_inode, req->port);


    if(dentry != NULL && dentry->inode != NULL)
    {
        char rstr[64], wstr[64];
        sprintf(rstr, "%d.in", req->pid);
        sprintf(wstr, "%d.out", req->pid);

        vfs_inode_t *r_in = vfs_create_inode(rstr, 0, dentry->inode, 0, 0);
        vfs_inode_t *w_in = vfs_create_inode(wstr, 0, dentry->inode, 0, 0);
        r_in->event_id = get_new_event_ID();
        r_in->handlers = list_create();
        r_in->type = VFS_PIPE;
        w_in->event_id = get_new_event_ID();
        w_in->handlers = list_create();
        w_in->type = VFS_PIPE;

        file_descriptor_t *desc = create_fd(current_thread->process);
        desc->mode = 0;
        desc->flags = O_RDWR;
        desc->permission = VFS_PERMISSION_READ | VFS_PERMISSION_WRITE;
        desc->read_descriptor->inode = r_in;
        desc->write_descriptor->inode = w_in;

        req->inodes[0] = w_in;
        req->inodes[1] = r_in;

        send_event(req->event_id);
        return desc;
    }
    else
        return NULL;
}

