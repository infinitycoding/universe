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


socket_request_t *port_fetch(struct process_state *process, const char *port)
{
    iterator_t i = iterator_create(process->socket_requests);
    socket_request_t *req;

    while(!list_is_last(&i))
    {
        req = (socket_request_t*) list_get_current(&i);
        if(strcmp(req->port,port) == 0)
        {
            return req;
        }
        list_next(&i);
    }

    return NULL;
}

file_descriptor_t *port_accept(struct process_state *process, socket_request_t *request)
{
    struct list_node *node = list_get_node_by_int(process->socket_requests, offsetof(socket_request_t, id), request->id);
    list_remove_node(node);

    vfs_dentry_t *dentry = vfs_get_dir_entry(process->socket_inode, request->port);

    if(dentry != NULL && dentry->inode != NULL)
    {
        char str[32];
        sprintf(str, "%d", request->pid);

        vfs_inode_t *inode = vfs_create_inode(str, 0, dentry->inode, 0, 0);
        inode->buffers[1] = block_buffer_create(0x1000);

        inode->event_id = get_new_event_ID();
        inode->handlers = list_create();
        inode->type = VFS_PIPE;

        file_descriptor_t *desc = create_fd(process);
        desc->flags = O_RDWR;
        desc->permission = VFS_PERMISSION_READ | VFS_PERMISSION_WRITE;

        // invert buffers
        desc->read_descriptor->inode = inode;
        desc->read_descriptor->read_buffer = 1;
        desc->read_descriptor->write_buffer = 0;

        desc->write_descriptor->inode = inode;
        desc->write_descriptor->read_buffer = 1;
        desc->write_descriptor->write_buffer = 0;

        request->inode = inode;

        send_event(request->event_id);
        return desc;
    }

    return NULL;
}

