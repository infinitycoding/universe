#ifndef _VFS_SOCKET_H
#define _VFS_SOCKET_H

/*
     Copyright 2012-2015 Infinitycoding all rights reserved
     This file is part of the Universe Kernel.

     The Universe Kernel is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.

     The Universe Kernel is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with the Universe Kernel. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @author Michael Sippel <micha@infinitycoding.de>
 */

#include <stdint.h>
#include <cpu.h>
#include <vfs/vfs.h>
#include <vfs/fd.h>

typedef struct socket_request
{
    int id;
    int pid;
    char *port;
    uint32_t event_id;

    vfs_inode_t *inode;
} socket_request_t;


void usys_connect(struct cpu_state **cpu);
void usys_readport(struct cpu_state **cpu);
void usys_accept(struct cpu_state **cpu);

socket_request_t *port_fetch(struct process_state *process, const char *port);
file_descriptor_t *port_accept(struct process_state *process, socket_request_t *request);

#endif

