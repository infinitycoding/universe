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

#include <mm/heap.h>
#include <mm/paging.h>
#include <memory_layout.h>

#include <sched/thread.h>
#include <sched/scheduler.h>

#include <clock.h>
#include <event/trigger.h>


extern struct thread_state *current_thread;

vfs_inode_t *root = NULL;
uint32_t nodes = 0;

/**
 * Initalizing the Virtual File System
 *
 * @param void
 * @return void
 */
void INIT_VFS(void)
{
    root = vfs_create_inode(NULL, S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO, NULL, 0, 0);

    vfs_inode_t *foo = vfs_create_inode("foo.txt", S_IRWXU | S_IRWXG | S_IRWXO, root, 0, 0);
    vfs_write(foo, 0, "Hallo Welt!\n", 13);
}

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
        inode->buffer = NULL;
    }
    else
    {
        inode->type = VFS_REGULAR;

        inode->buffer = (vfs_buffer_info_t*) malloc(sizeof(vfs_buffer_info_t));
        inode->buffer->num_blocks = 0;
        inode->buffer->blocks = list_create();

        if(S_ISDIR(inode->stat))
            inode->buffer->block_size = sizeof(vfs_dentry_t);
        else
            inode->buffer->block_size = 0x1000;
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
        current->buffer = inode;

        vfs_inode_t *par = vfs_create_inode("..", S_IFLNK, inode, 0, 0);
        par->buffer = parent;
    }

    return inode;
}

vfs_inode_t *vfs_create_pipe(uid_t uid, gid_t gid)
{
    vfs_inode_t *inode = vfs_create_inode(NULL, 0, NULL, uid, gid);

    inode->buffer->event_id = get_new_event_ID();
    inode->buffer->handlers = list_create();

    inode->type = VFS_PIPE;

    return inode;
}

/**
 * Create a new directory entry
 *
 * @param entry_inode the node that will be entried
 * @return the new directory entry
 */
vfs_dentry_t* vfs_create_dir_entry(vfs_inode_t *entry_inode)
{
    vfs_dentry_t *dir_ent = malloc(sizeof(vfs_dentry_t));

    dir_ent->ino = entry_inode->stat.st_ino;
    dir_ent->inode = entry_inode;

    return dir_ent;
}

void vfs_add_dir_entry(vfs_inode_t *dir, vfs_dentry_t *entry)
{
    if(! S_ISDIR(dir->stat))
        return;

    vfs_buffer_info_t *info = dir->buffer;
    if(info == NULL)
    {
        printf("leerer puffer\n");
        return;
    }

    vfs_buffer_block_t *block = malloc(sizeof(vfs_buffer_block_t));
    block->base = entry;
    block->block_id = info->num_blocks++;
    list_push_back(info->blocks, block);
}

void vfs_remove_dir_entry(vfs_inode_t *dir, vfs_inode_t *inode)
{
    if(! S_ISDIR(dir->stat))
        return;

    vfs_buffer_info_t *info = dir->buffer;
    if(info == NULL)
    {
        printf("leerer puffer\n");
        return;
    }

    iterator_t it = iterator_create(info->blocks);
    int i;
    for(i = 0; i < info->num_blocks; i++)
    {
        vfs_buffer_block_t *block = it.current->element;
        vfs_dentry_t *dentry = block->base;

        if(dentry->inode == inode)
        {
            list_remove(&it);
            info->num_blocks--;
        }

        list_next(&it);
    }
}

vfs_dentry_t *vfs_get_dir_entry(vfs_inode_t *ino, const char *name)
{
    GET_INODE(ino);
    if(S_ISDIR(ino->stat))
    {
        vfs_buffer_info_t *info = ino->buffer;

        if(! list_is_empty(info->blocks))
        {
            iterator_t it = iterator_create(info->blocks);
            list_set_first(&it);

            int i;
            for(i = 0; i < info->num_blocks; i++)
            {
                vfs_buffer_block_t *block = (vfs_buffer_block_t*) it.current->element;
                vfs_dentry_t *dentry = (vfs_dentry_t*) block->base;

                if(strcmp(name, dentry->inode->name) == 0)
                {
                    return dentry;
                }

                list_next(&it);
            }
        }
    }

    return NULL;
}

/**
 * write into a vfs-inode
 *
 * @param inode the inode, in that will be written
 * @param off offset in the inode
 * @param base pointer to the data
 * @param bytes number of bytes to write
 *
 * @return number of written bytes
 */
int vfs_write(vfs_inode_t *inode, int off, void *buffer, int bytes)
{
    GET_INODE(inode);

    if(inode == NULL || inode->buffer == NULL)
    {
        printf("trying to write into uninitalized buffer.\n");
        return 0;
    }

    vfs_buffer_info_t *info = inode->buffer;
    vfs_buffer_block_t *block = NULL;

    // calculate block indices
    int block_id  = off / info->block_size;
    int block_off = off % info->block_size;

    // search first block
    block = vfs_get_buffer_block(info, block_id);

    uint8_t *data = (uint8_t*) buffer;
    int index = block_off;

    // go through all bytes...
    int i;
    for(i = 0; i < bytes; i++)
    {
        // if the block ends, go to the next
        if(index >= info->block_size)
        {
            block_id++;
            block = vfs_get_buffer_block(info, block_id);
            index = 0;
        }

        // if nothing found, create a new block
        if(block == NULL)
        {
            block = malloc(sizeof(vfs_buffer_block_t));
            block->base = malloc(info->block_size);
            block->block_id = info->num_blocks++;
            list_push_back(info->blocks, block);

            block_id = block->block_id;
            index = 0;
        }

        // copy data
        block->base[index++] = data[i];
    }

    // increase length
    if( (off + i) > inode->length)
    {
        inode->length = off + i;
        inode->stat.st_size = inode->length;
    }

    // pipes send an event signal
    if(inode->type == VFS_PIPE)
    {
        send_event(info->event_id);
        info->event_id = get_new_event_ID();

        launch_pipe_handlers(info);
    }

    return i;
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

    if(inode == NULL || inode->buffer == NULL)
    {
        printf("trying to read from uninitalized buffer.\n");
        return 0;
    }

    if(inode->length >= offset)
    {
        vfs_buffer_info_t *info = inode->buffer;
        vfs_buffer_block_t *block = NULL;
        //iterator_t it = iterator_create(info->blocks);

        int block_id  = offset / info->block_size;
        int block_off = offset % info->block_size;

        // search first block
        block = vfs_get_buffer_block(info, block_id);

        uint8_t *data = (uint8_t*) buffer;
        int index = block_off;

        // go through all bytes...
        int i;
        for(i = 0; i < bytes; i++)
        {
            // if the block ends, go to the next
            if(index >= info->block_size)
            {
                block_id++;
                index = 0;
                block = vfs_get_buffer_block(info, block_id);

                if(block == NULL)
                    break;
            }

            if(inode->length <= (offset+i))
                break;

            // copy data
            data[i] = block->base[index++];
        }

        return i;
    }

    return 0;
}

/**
 * get stat of a node
 *
 * @param node node
 * @param buffer buffer
 *
 * @return _SUCCESS
 */
int vfs_stat(vfs_inode_t *node, struct stat *buffer)
{
    GET_INODE(node);

    uint8_t *node_stat = (uint8_t*) &node->stat;
    uint8_t *buf = (uint8_t*) buffer;
    int i = 0;
    while (i++ < sizeof(struct stat))
    {
        *buf++ = *node_stat++;
    }

    return 0;
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

/**
 * Dissolve a path to an inode
 *
 * @param path
 * @return inode
 */
vfs_inode_t *vfs_lookup_path(char *path)
{
    vfs_inode_t *parent = root;

    if(path[0] != '/')
    {
        parent = current_thread->process->cwd;
    }
    else
    {
        path++;
    }

    if(path[0] == '\0')
        return parent;

    int len = strlen(path);
    if(path[len-1] == '/')
    {
        path[len-1] = '\0';
    }

    char delimiter[] = "/";
    char *str = (char*) strtok(path, delimiter);
    while(str != NULL)
    {
        vfs_inode_t *real = parent;
        GET_INODE(real);

        vfs_buffer_info_t *info = real->buffer;
        iterator_t it = iterator_create(info->blocks);
        int i;
        int found = 0;
        for(i = 0; i < info->num_blocks; i++)
        {
            vfs_buffer_block_t *block = it.current->element;
            vfs_dentry_t *dentry = (vfs_dentry_t*) block->base;

            if(strcmp(str, dentry->inode->name) == 0)
            {
                parent = dentry->inode;
                found = 1;
                break;
            }

            list_next(&it);
        }

        if(!found)
        {
            return NULL;
        }
        else
        {
            str = strtok(NULL, delimiter);
        }
    }

    return parent;
}

/**
 * Dissolve a path to an inode and create all missing nodes
 *
 * @param path
 * @param mode
 * @param uid
 * @param gid
 * @return inode
 */
vfs_inode_t *vfs_create_path(char *path, mode_t mode, uid_t uid, gid_t gid)
{
    vfs_inode_t *parent = root;

    if(path[0] != '/')
    {
        parent = current_thread->process->cwd;
    }
    else
    {
        path++;
    }

    if(path[0] == '\0')
        return parent;

    int len = strlen(path);
    if(path[len-1] == '/')
    {
        path[len-1] = '\0';
    }

    char delimiter[] = "/";
    char *str = (char*) strtok(path, delimiter);
    while(str != NULL)
    {
        vfs_inode_t *real = parent;
        GET_INODE(real);

        vfs_buffer_info_t *info = real->buffer;
        iterator_t it = iterator_create(info->blocks);
        int i;
        int found = 0;
        for(i = 0; i < info->num_blocks; i++)
        {
            vfs_buffer_block_t *block = it.current->element;
            vfs_dentry_t *dentry = (vfs_dentry_t*) block->base;

            if(strcmp(str, dentry->inode->name) == 0)
            {
                parent = dentry->inode;
                found = 1;
                break;
            }

            list_next(&it);
        }

        char *new_str = strtok(NULL, delimiter);
        if(!found)
        {
            int n_mode;
            if(new_str != NULL)
                n_mode = mode & (S_IRWXU | S_IRWXG | S_IRWXO) | S_IFDIR;
            else
                n_mode = mode;

            if(vfs_access(parent, W_OK, uid, gid) == 0)
            {
                parent = vfs_create_inode(str, n_mode, parent, uid, gid);
            }
            else
            {
                return NULL;
            }
        }
        str = new_str;
    }

    return parent;
}

#ifdef VFS_DEBUG
void vfs_debug_output_all()
{
    vfs_debug_output(root);
}

void vfs_debug_output(vfs_inode_t *start)
{
    vfs_inode_t *current;

    int num = start->length / sizeof(vfs_dentry_t);
    vfs_dentry_t *entries = malloc(start->length);
    vfs_read(start, 0, entries, start->length);

    int i;
    for(i = 0; i < num; i++)
    {
        if(S_ISDIR(entries[i].inode->stat))
        {
            vfs_debug_output(entries[i].inode);
        }
        else
        {
            current = entries[i].inode;

            while(current != root)
            {
                printf("%s in ", current->name);
                current = current->parent;
            }

            printf("root\n");
        }
    }

    free(entries);
}
#endif

void launch_pipe_handlers(vfs_buffer_info_t *pipe)
{
    struct list_node *node = pipe->handlers->head->next;
    struct list_node *head = pipe->handlers->head;

    while(node != head)
    {
        vfs_pipe_trigger_t *trigger = (vfs_pipe_trigger_t*) node->element;
        thread_create(current_thread->process, USERMODE, trigger->eip, trigger->argv, trigger->environ, 0, NULL);

        node = node->next;
    }
}

