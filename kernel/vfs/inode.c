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

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include <printf.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include <mm/heap.h>
#include <mm/paging.h>
#include <memory_layout.h>

#include <sched/thread.h>
#include <sched/scheduler.h>

#include <clock.h>
#include <event/trigger.h>

/**
 * Create a new vfs-node
 *
 * @param name name for the new node
 * @param flags flags for the new node
 * @param parent the parent node for the new node.
 *               if parent is NULL, the parent for the node is root.
 *
 * @return pointer to the new node
 */
vfs_inode_t* vfs_create_inode(char *name, mode_t mode, vfs_inode_t *parent, uid_t uid, gid_t gid)
{
    static uint32_t nodes = 0;


    vfs_inode_t *inode = malloc(sizeof(vfs_inode_t));

    if(name != NULL)
    {
        inode->name = (char*) malloc(strlen(name)+1);
        strcpy(inode->name, name);
    }

    inode->length = 0;

    inode->stat.st_mode = mode;
    inode->stat.st_ino = nodes++;
    inode->stat.st_uid = uid;
    inode->stat.st_gid = gid;
    update_time((struct time*)&inode->stat.st_atime);
    update_time((struct time*)&inode->stat.st_mtime);

    if(mode & S_IFLNK)
    {
        inode->type = VFS_LINK;
        inode->read_buffer = NULL;
        inode->write_buffer = NULL;
    }
    else
    {
        inode->type = VFS_REGULAR;

        block_buffer_info_t *buf = NULL;

        if(S_ISDIR(inode->stat))
            buf = block_buffer_create(sizeof(vfs_dentry_t));
        else
            buf = block_buffer_create(0x1000);

        inode->read_buffer = buf;
        inode->write_buffer = buf;
    }

    // write inode into parents entries
    if(parent != NULL)
    {
        inode->parent = parent;
        vfs_dentry_t *entry = vfs_create_dir_entry(inode);
        vfs_add_dir_entry(inode->parent, entry);
    }

    // write "." and ".." entries
    if(S_ISDIR(inode->stat))
    {
        vfs_inode_t *current = vfs_create_inode(".", S_IFLNK, inode, 0, 0);
        current->read_buffer = inode;

        vfs_inode_t *par = vfs_create_inode("..", S_IFLNK, inode, 0, 0);
        par->read_buffer = parent;
    }

    return inode;
}

vfs_inode_t *vfs_create_pipe(uid_t uid, gid_t gid)
{
    vfs_inode_t *inode = vfs_create_inode(NULL, 0, NULL, uid, gid);

    inode->event_id = get_new_event_ID();
    inode->handlers = list_create();

    inode->type = VFS_PIPE;

    return inode;
}

/**
 * write into a vfs-inode
 *
 * @param inode the inode, in that will be written
 * @param offset offset in the inode
 * @param base pointer to the data
 * @param bytes number of bytes to write
 *
 * @return number of written bytes
 */
int vfs_write(vfs_inode_t *inode, int offset, void *buffer, int bytes)
{
    GET_INODE(inode);

    if(inode == NULL || inode->write_buffer == NULL)
    {
        printf("trying to write into uninitalized buffer.\n");
        return 0;
    }

    int len = block_buffer_write(inode->write_buffer, offset, (uint8_t*) buffer, bytes);

    // increase length
    if( (offset + len) > inode->length)
    {
        inode->length = offset + len;
        inode->stat.st_size = inode->length;
    }

    // pipes send an event signal
    if(inode->type == VFS_PIPE)
    {
        send_event(inode->event_id);
        inode->event_id = get_new_event_ID();

        launch_pipe_handlers(inode);
    }

    return len;
}

/**
 * read the content from a vfs-node
 *
 * @param node the node, that will be readed
 * @param offset the offset from the node
 *
 * @return number of bytes
 */
int vfs_read(vfs_inode_t *inode, int offset, void *buffer, int bytes)
{
    GET_INODE(inode);

    if(inode == NULL || inode->read_buffer == NULL)
    {
        printf("trying to read from uninitalized buffer.\n");
        return 0;
    }

    int b = bytes;
    if( (offset + bytes) > inode->length)
        b = inode->length - offset;

    return block_buffer_read(inode->read_buffer, offset, (uint8_t*) buffer, b);
}


/**
 * check the access of a node
 *
 * @param node node
 * @param modus access to check
 *
 * @return
 */
int vfs_access(vfs_inode_t *node, mode_t modus, uid_t uid, gid_t gid)
{
    GET_INODE(node);

    if (node->stat.st_uid == uid)
    {
        if ((modus & R_OK) &&
                !(node->stat.st_mode & S_IRUSR))
            return -1;
        if ((modus & W_OK) &&
                !(node->stat.st_mode & S_IWUSR))
            return -1;
        if ((modus & X_OK) &&
                !(node->stat.st_mode & S_IXUSR))
            return -1;
    }
    else if (node->stat.st_gid == gid)
    {
        if ((modus & R_OK) &&
                !(node->stat.st_mode & S_IRGRP))
            return -1;
        if ((modus & W_OK) &&
                !(node->stat.st_mode & S_IWGRP))
            return -1;
        if ((modus & X_OK) &&
                !(node->stat.st_mode & S_IXGRP))
            return -1;
    }
    else
    {
        if ((modus & R_OK) &&
                !(node->stat.st_mode & S_IROTH))
            return -1;
        if ((modus & W_OK) &&
                !(node->stat.st_mode & S_IWOTH))
            return -1;
        if ((modus & X_OK) &&
                !(node->stat.st_mode & S_IXOTH))
            return -1;
    }

    return 0;
}

