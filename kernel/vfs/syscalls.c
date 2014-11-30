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

#include <stdint.h>
#include <string.h>
#include <sched/scheduler.h>
#include <mm/heap.h>
#include <printf.h>
#include <sys/unistd.h>
#include <vfs/vfs.h>
#include <vfs/fd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <memory_layout.h>
#include <mm/paging.h>
#include <sched/thread.h>
#include <sched/process.h>
#include <event/trigger.h>
#include <drivers/clock.h>

extern struct thread_state *current_thread;
extern vfs_inode_t *root;

// Systemcalls
struct fd *get_fd(struct process_state *process, int fd)
{
    iterator_t it = iterator_create(process->files);
    while(!list_is_empty(process->files) && !list_is_last(&it))
    {
        struct fd *desc = list_get_current(&it);
        if(desc->id == fd)
        {
            return desc;
        }
        else
        {
            list_next(&it);
        }
    }

    return NULL;
}

struct fd *create_fd(struct process_state *process)
{
    struct fd *desc = malloc(sizeof(struct fd));

    memset(desc, 0, sizeof(struct fd));
    desc->id = list_length(process->files);

    list_push_back(process->files, desc);

    return desc;
}

void sys_open(struct cpu_state **cpu)
{
    char *path = (char *) (*cpu)->CPU_ARG1;
    int oflags = (*cpu)->CPU_ARG2;
    mode_t mode = (*cpu)->CPU_ARG3;
    vfs_inode_t *inode = vfs_lookup_path(path);

    if(inode == NULL)
    {
        if(oflags & O_CREAT)
        {
            // create inode
            inode = vfs_create_path(path, mode, current_thread->process->uid, current_thread->process->gid);
			if(inode == NULL)
            {
                (*cpu)->CPU_ARG0 = _NO_PERMISSION;
                return;
            }
        }
        else
        {
            (*cpu)->CPU_ARG0 = _FAILURE;
            return;
        }
    }
    else
    {
        if(oflags & O_EXCL)
        {
            (*cpu)->CPU_ARG0 = _FAILURE;
            return;
        }
    }

    if(oflags & O_TRUNC)
    {
        if(vfs_access(inode, W_OK, current_thread->process->uid, current_thread->process->gid) == 0)
        {
            // TODO: clear file
        }
        else
        {
            (*cpu)->CPU_ARG0 = _NO_PERMISSION;
            return;
        }
    }

    struct fd *desc = create_fd(current_thread->process);
    desc->mode = mode;
    desc->flags = oflags;

    if(oflags & O_APPEND)
    {
        desc->read_pos = inode->length;
        desc->write_pos = inode->length;
    }

    if( (oflags & O_RDONLY || oflags & O_RDWR) && vfs_access(inode, R_OK, current_thread->process->uid, current_thread->process->gid) == 0)
    {
        desc->permission |= VFS_PERMISSION_READ;
        desc->read_inode = inode;
    }

    if( (oflags & O_WRONLY || oflags & O_RDWR) && vfs_access(inode, W_OK, current_thread->process->uid, current_thread->process->gid) == 0)
    {
        desc->permission |= VFS_PERMISSION_WRITE;
        desc->write_inode = inode;
    }

    (*cpu)->CPU_ARG0 = desc->id;
}

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

void sys_mknod(struct cpu_state **cpu)
{
    char *path = (char *) (*cpu)->CPU_ARG1;
    int mode =(int) (*cpu)->CPU_ARG2;
    //int dev = (int)(*cpu)->CPU_ARG3; // unused

    vfs_inode_t *inode = vfs_create_path(path, mode, current_thread->process->uid, current_thread->process->gid);

    if(inode != NULL)
    {
        if(S_ISFIFO(inode->stat))
        {
            inode->buffer->event_id = get_new_event_ID();
            inode->buffer->handlers = list_create();
            inode->type = VFS_PIPE;
        }

        (*cpu)->CPU_ARG0 = _SUCCESS;
    }
    else
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
    }
}

void sys_close(struct cpu_state **cpu)
{
    int fd = (*cpu)->CPU_ARG1;

    struct list_node *node = current_thread->process->files->head->next;
    int i;
    for(i = 0; i < list_length(current_thread->process->files); i++)
    {
        struct fd *desc = node->element;
        if(desc->id == fd)
        {
            list_remove_node(node);
            (*cpu)->CPU_ARG0 = _SUCCESS;
            return;
        }
        else
        {
            node = node->next;
        }
    }

    (*cpu)->CPU_ARG0 = _FAILURE;
}

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
            vfs_buffer_info_t *info = real->buffer;
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
                    add_trigger(WAIT_EVENT, info->event_id, 0, current_thread, sys_read);
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

void sys_write(struct cpu_state **cpu)
{
    int fd = (*cpu)->CPU_ARG1;
    char *buf = (void*) (*cpu)->CPU_ARG2;
    size_t len = (*cpu)->CPU_ARG3;

    if(fd == 2)   // TODO FIXME
    {
        int i;
        for(i = 0; i < len; i++)
        {
            printf("%c", buf[i]);
        }
        (*cpu)->CPU_ARG0 = _SUCCESS;
        return;
    }

    struct fd *desc = get_fd(current_thread->process, fd);
    if(desc != NULL)
    {
        if(desc->permission & VFS_PERMISSION_WRITE && desc->write_inode != NULL)
        {
            vfs_inode_t *inode = desc->write_inode;
			vfs_inode_t *real = inode;
			GET_INODE(real);
			if(S_ISDIR(real->stat))
			{
				(*cpu)->CPU_ARG0 = _NO_PERMISSION;
				return;
			}

            int ret = vfs_write(inode, desc->write_pos, buf, len);
            (*cpu)->CPU_ARG0 = ret;

            if(ret > 0)
            {
                desc->write_pos += len;
                if(real->type != VFS_PIPE)
                {
                    desc->read_pos += len;
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

void sys_create(struct cpu_state **cpu)
{
    char *name = (char *)(*cpu)->CPU_ARG1;
    int mode = (*cpu)->CPU_ARG2;

    vfs_inode_t *inode = vfs_create_path(name, mode, current_thread->process->uid, current_thread->process->gid);

    if(inode != NULL)
    {
        struct fd *desc = create_fd(current_thread->process);
        desc->mode = mode;
        desc->flags = O_RDWR;
        desc->permission = VFS_PERMISSION_READ | VFS_PERMISSION_WRITE;
        desc->read_inode = inode;
        desc->write_inode = inode;

        (*cpu)->CPU_ARG0 = desc->id;
    }
    else
    {
        (*cpu)->CPU_ARG0 = _FAILURE;
    }
}

void sys_socket(struct cpu_state **cpu)
{
    //int domain = (*cpu)->CPU_ARG1;
    //int type = (*cpu)->CPU_ARG2;
    //int protocol = (*cpu)->CPU_ARG3;

    struct fd *desc = create_fd(current_thread->process);
    (*cpu)->CPU_ARG0 = desc->id;
}

void sys_link(struct cpu_state **cpu)
{
    char *src_path = (char *)(*cpu)->CPU_ARG1;
    char *dest_path = (char *)(*cpu)->CPU_ARG2;

    vfs_inode_t *src_inode = vfs_lookup_path(src_path);

    // FIXME: only works in root
    vfs_inode_t *dest_parent = root; // FIXME TODO

    if(src_inode != NULL && dest_parent != NULL)
    {
        if(vfs_access(src_inode, R_OK, current_thread->process->uid, current_thread->process->gid == 0) &&
                vfs_access(dest_parent, W_OK, current_thread->process->uid, current_thread->process->gid == 0))
        {
            vfs_inode_t *dest_inode = vfs_create_inode(dest_path, src_inode->stat.st_mode | S_IFLNK, dest_parent, current_thread->process->uid, current_thread->process->gid);

            if(dest_inode != NULL)
            {
                dest_inode->buffer = (void *)dest_inode;

                (*cpu)->CPU_ARG0 = _SUCCESS;
            }
            else
            {
                (*cpu)->CPU_ARG0 = _FAILURE;
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

void sys_unlink(struct cpu_state **cpu)
{
    char *path = (char *)(*cpu)->CPU_ARG1;

    vfs_inode_t *link = vfs_lookup_path(path);
    if(link != NULL)
    {
        if(vfs_access(link, W_OK, current_thread->process->uid, current_thread->process->gid == 0))
        {
            link->buffer = NULL;
            link->type = VFS_REGULAR;
            free(link);
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

	while(inode != root) {
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

void set_pipe_trigger(struct cpu_state **cpu)
{
    int fd = (*cpu)->CPU_ARG1;
    struct fd *desc = get_fd(current_thread->process, fd);
    vfs_inode_t *inode = desc->read_inode;

    if(desc->permission & VFS_PERMISSION_READ)
    {
        vfs_buffer_info_t *pipe = inode->buffer;

        vfs_pipe_trigger_t *trigger = malloc(sizeof(vfs_pipe_trigger_t));
        trigger->eip = (*cpu)->CPU_ARG2;
        trigger->argc = (*cpu)->CPU_ARG3;
        trigger->argv = (char**)(*cpu)->CPU_ARG4;
        trigger->environ = (char**)(*cpu)->CPU_ARG5;
        list_push_back(pipe->handlers, (void*) trigger);

        (*cpu)->CPU_ARG0 = _SUCCESS;
    }
    else
    {
        (*cpu)->CPU_ARG0 = _NO_PERMISSION;
    }
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
    if(node->stat.st_uid != current_thread->process->uid)
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


void usys_connect(struct cpu_state **cpu)
{
	int pid = (int) (*cpu)->CPU_ARG1;
	int port = (int) (*cpu)->CPU_ARG2;

	

    (*cpu)->CPU_ARG0 = _SUCCESS;
}

