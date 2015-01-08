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

// Systemcalls
void sys_seek(struct cpu_state **cpu)
{
    int fd = (*cpu)->CPU_ARG1;
    int off = (*cpu)->CPU_ARG2;
    int whence = (*cpu)->CPU_ARG3;

    struct fd *file = get_fd(current_thread->process, fd);

    switch(whence)
    {
        case SEEK_SET: // absolute
            file->read_pos = off;
            file->write_pos = off;
            break;
        case SEEK_CUR: // relative from current position
            file->read_pos += off;
            file->write_pos += off;
            break;
        case SEEK_END: // relative from end
            if(file->read_inode != NULL)
                file->read_pos = file->read_inode->length - off;
            if(file->write_inode != NULL)
                file->write_pos = file->write_inode->length - off;
            break;
        default: // ???
            (*cpu)->CPU_ARG0 = _FAILURE;
            return;
    }

    (*cpu)->CPU_ARG0 = file->read_pos;
}

void sys_stat(struct cpu_state **cpu)
{
    if((*cpu)->CPU_ARG1 == 0 || (*cpu)->CPU_ARG2 == 0)
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
        return;
    }

    vfs_inode_t *node = vfs_lookup_path((char*)(*cpu)->CPU_ARG1);
    if(node == NULL)
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
        return;
    }

    (*cpu)->CPU_ARG0 = vfs_stat(node,(struct stat *)(*cpu)->CPU_ARG2);
}


void sys_fstat(struct cpu_state **cpu)
{
    if((*cpu)->CPU_ARG1 == 0)
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
        return;
    }

    iterator_t file_it = iterator_create(current_thread->process->files);
    while(!list_is_empty(current_thread->process->files) && !list_is_last(&file_it))
    {
        struct fd *file = list_get_current(&file_it);
        if(file->id == (*cpu)->CPU_ARG1)
        {
            (*cpu)->CPU_ARG0 = vfs_stat(file->read_inode, (struct stat *)(*cpu)->CPU_ARG2);
            return;
        }
        list_next(&file_it);
    }

    (*cpu)->CPU_ARG0 = _FAILURE;
}

void sys_chmod(struct cpu_state **cpu)
{
    // Check if path is not NULL
    char *file = (char *)(*cpu)->CPU_ARG1;
    if(file == NULL)
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
        return;
    }
    // Lookup path
    vfs_inode_t *node = vfs_lookup_path(file);
    if(node == NULL)
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
        return;
    }
    // Check permissions
    if(node->stat.st_uid != current_thread->process->uid)
    {
        (*cpu)->CPU_ARG0 = _NO_PERMISSION;
        return;
    }
    // change mode
    node->stat.st_mode = (*cpu)->CPU_ARG2;
    (*cpu)->CPU_ARG0 = _SUCCESS;
}


void sys_rename(struct cpu_state **cpu)
{
    // Check if path is not NULL
    char *file = (char *)(*cpu)->CPU_ARG1;
    char *new_filename = (char *)(*cpu)->CPU_ARG2;
    if(file == NULL)
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
        return;
    }

    // Lookup path
    vfs_inode_t *node = vfs_lookup_path(file);
    if(node == NULL)
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
        return;
    }

    if(vfs_access(node, W_OK, current_thread->process->uid, current_thread->process->gid) != 0)
    {
        (*cpu)->CPU_ARG0 = _NO_PERMISSION;
        return;
    }

    //creante new namebuffer
    size_t bufferlen = strlen(new_filename);
    char *namebuffer = (char *) malloc(bufferlen+1); // bufferlen + zero terminator
    namebuffer[bufferlen] = '\0';
    strncpy(namebuffer,new_filename,bufferlen);
    free(node->name);
    node->name = namebuffer;

    (*cpu)->CPU_ARG0 = _SUCCESS;
}

void sys_lchown(struct cpu_state **cpu)
{
    char *file = (char *)(*cpu)->CPU_ARG1;
    if(file == NULL)
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
        return;
    }
    // Lookup path
    vfs_inode_t *node = vfs_lookup_path(file);
    if(node == NULL)
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
        return;
    }
    // Check permissions
    if(node->stat.st_uid != current_thread->process->uid || current_thread->process->uid == 0)
    {
        (*cpu)->CPU_ARG0 = _NO_PERMISSION;
        return;
    }

    // do to follow link
    if(node->type == VFS_LINK)
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
        return;
    }

    // TODO: change  the  permissions in filedescriptors
    node->stat.st_uid = (uid_t) (*cpu)->CPU_ARG2;
    node->stat.st_gid = (gid_t) (*cpu)->CPU_ARG3;
    (*cpu)->CPU_ARG0 = _SUCCESS;
}

void sys_access(struct cpu_state **cpu)
{
    char *file = (char *) (*cpu)->CPU_ARG1;
    mode_t mode = (mode_t) (*cpu)->CPU_ARG2;

    if(file == NULL)
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
        return;
    }

    vfs_inode_t *node = vfs_lookup_path(file);
    if(node == NULL)
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
        return;
    }

    (*cpu)->CPU_ARG0 = vfs_access(node, mode, current_thread->process->uid, current_thread->process->gid);
}



