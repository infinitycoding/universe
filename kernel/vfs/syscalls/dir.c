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


void sys_chdir(struct cpu_state **cpu)
{
    char *path = (char *)(*cpu)->CPU_ARG1;

    vfs_inode_t *nwd = vfs_lookup_path(path);
    if(nwd != NULL)
    {
        GET_INODE(nwd);
        if(nwd != NULL && S_ISDIR(nwd->stat))
        {
            if(vfs_access(nwd, R_OK, current_thread->process->uid, current_thread->process->gid) == 0)
            {
                current_thread->process->cwd = nwd;
                (*cpu)->CPU_ARG0 = _SUCCESS;
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
    else
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
    }
}

void sys_getdents(struct cpu_state **cpu)
{
    int fd = (*cpu)->CPU_ARG1;
    dirent_t *dentry = (dirent_t *)(*cpu)->CPU_ARG2;

    vfs_inode_t *parent = get_fd(current_thread->process, fd)->read_inode;
    GET_INODE(parent);

    if(vfs_access(parent, R_OK, current_thread->process->uid, current_thread->process->gid) == 0)
    {
        if(! S_ISDIR(parent->stat))
        {
            (*cpu)->CPU_ARG0 = _NO_PERMISSION;
            return;
        }

        vfs_buffer_info_t *info = parent->buffer;

        if(current_thread->getdents_pos < info->num_blocks && (fd == current_thread->getdents_old_fd || current_thread->getdents_old_fd == -1))
        {
            vfs_buffer_block_t *block = vfs_get_buffer_block(info, current_thread->getdents_pos++);
            vfs_dentry_t *entry = (vfs_dentry_t *)block->base;
            vfs_inode_t *ino = entry->inode;

            strcpy(dentry->name, ino->name);
            memcpy(&dentry->stat, &ino->stat, sizeof(struct stat));
            dentry->id = ino->stat.st_ino;
            (*cpu)->CPU_ARG0 = (uint32_t) dentry;
        }
        else
        {
            current_thread->getdents_pos = 0;
            (*cpu)->CPU_ARG0 = (uint32_t) NULL;
        }

        current_thread->getdents_old_fd = fd;
    }
    else
    {
        (*cpu)->CPU_ARG0 = _NO_PERMISSION;
    }
}

void sys_mkdir(struct cpu_state **cpu)
{
    char *path = (char *)(*cpu)->CPU_ARG1;
    int mode = (int)(*cpu)->CPU_ARG2;

    vfs_create_path(path, mode | S_IFDIR, current_thread->process->uid, current_thread->process->gid);

    (*cpu)->CPU_ARG0 = _SUCCESS;

    return;
}

void sys_getcwd(struct cpu_state **cpu)
{
    char *buffer = (char *)(*cpu)->CPU_ARG1;
    int buffersize = (int)(*cpu)->CPU_ARG2;
    //int currentend = 0;
    int cwdsize = 0;				// the final size of the final '\0'

    vfs_inode_t *inode = current_thread->process->cwd;

    while(inode != root)
    {
        cwdsize += strlen((char*)inode->name) + 1;
        inode = inode->parent;
    }

    if(buffer == NULL)
    {
        (*cpu)->CPU_ARG0 = 0;
        return;
    }

    if(buffersize < cwdsize)
    {
        (*cpu)->CPU_ARG0 = 0;
        return;
    }

    memset(buffer, 0, buffersize);
    if(cwdsize > 0)
    {
        buffer += cwdsize+1;
        *--buffer = '\0';

        inode = current_thread->process->cwd;
        while(inode != root)
        {
            int len = strlen((char*)inode->name);
            buffer -= len;
            memcpy(buffer, inode->name, len);
            *--buffer = '/';
            inode = inode->parent;
        }
    }
    else
    {
        strcpy(buffer,"/");
    }

    (*cpu)->CPU_ARG0 =(unsigned int) buffer;
    return;
}

