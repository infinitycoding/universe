/*
	Copyright 2012 universe coding group (UCG) all rights reserved
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
	@author Michael Sippel <michamimosa@gmail.com>
*/

#include <stdint.h>
#include <string.h>
#include <scheduler.h>
#include <heap.h>
#include <printf.h>
#include <unistd.h>
#include <vfs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <memory_layout.h>
#include <paging.h>
#include <thread.h>
#include <trigger.h>

extern struct thread_state *current_thread;

vfs_inode_t *root = NULL;
uint32_t nodes = 0;

// solve a link
#define GET_INODE(i) if(i->type == VFS_LINK) i = (vfs_inode_t*) i->base;

/**
 * Initalizing the Virtual File System
 *
 * @param void
 * @return void
 */
void INIT_VFS(void) {
	root = malloc(sizeof(vfs_inode_t));
	root->stat.st_ino = 0;
	root->stat.st_mode = 0x1ff;
	nodes = 1;
	root->name = "root";
	root->length = 0;
	root->parent = NULL;
	root->type = VFS_REGULAR;

	vfs_inode_t *foo = vfs_create_inode("foo.txt", 0x1ff, root, 0, 0);
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
vfs_inode_t* vfs_create_inode(char *name, mode_t mode, vfs_inode_t *parent, uid_t uid, gid_t gid) {
	vfs_inode_t *node = malloc(sizeof(vfs_inode_t));
	node->name = name;
	node->length = 0;
	node->type = VFS_REGULAR;
	node->base = NULL;
	if (parent != NULL) {
		node->parent = parent;
		vfs_dentry_t *entry = vfs_create_dir_entry(node);
		vfs_write(parent, parent->length, entry, sizeof(vfs_dentry_t));
	}

	node->stat.st_mode = mode;
	node->stat.st_ino = nodes++;
	node->stat.st_uid = uid;
	node->stat.st_gid = gid;
	update_time((struct time*)&node->stat.st_atime);
	update_time((struct time*)&node->stat.st_mtime);
	return node;
}

vfs_inode_t *vfs_create_pipe(uid_t uid, gid_t gid) {
	vfs_inode_t *inode = vfs_create_inode("pipe", 0, NULL, uid, gid);

	vfs_pipe_info_t *pipe = malloc(sizeof(vfs_pipe_info_t));
	pipe->num_readers = 1;
	pipe->num_writers = 1;
	pipe->pipe_buffer = list_create();
	pipe->event_id = get_new_event_ID();
	pipe->handlers = list_create();

	inode->type = VFS_PIPE;
	inode->base = pipe;
	inode->length = 0;

	return inode;
}

/**
 * Create a new directory entry
 *
 * @param entry_inode the node that will be entried
 * @return the new directory entry
 */
vfs_dentry_t* vfs_create_dir_entry(vfs_inode_t *entry_inode) {
	vfs_dentry_t *dir_ent = malloc(sizeof(vfs_dentry_t));

	dir_ent->ino = entry_inode->stat.st_ino;
	dir_ent->inode = entry_inode;

	return dir_ent;
}

/**
 * write into a vfs-node
 *
 * @param node the node, in that will be written
 * @param base pointer to the data
 *
 * @return number of written bytes
 */
int vfs_write(vfs_inode_t *node, int off, void *base, int bytes) {
	GET_INODE(node);

	int old_len = node->length;
	if( (off + bytes) > node->length) {
		node->length = off + bytes;
		node->stat.st_size = node->length;
	}
	
	if(node->type == VFS_PIPE) {
		int block_id = off / PAGE_SIZE;
		int block_off= off % PAGE_SIZE;

		vfs_pipe_info_t *info = (vfs_pipe_info_t*) node->base;

		vfs_pipe_buffer_block_t *block = NULL;
		struct list_node *bn = info->pipe_buffer->head->next;
		int i,found = 0;
		for(i = 0; i < info->num_blocks; i++) {
			block = (vfs_pipe_buffer_block_t*) bn->element;
			if(block->block_id == block_id) {
				found = 1;
				break;
			}
			bn = bn->next;
		}

		uint8_t *data = (uint8_t*) base;
                int index = block_off;
		for(i = 0; i < bytes; i++) {
			if(index >= PAGE_SIZE) {
				block_id++;
				if(block_id >= info->num_blocks) {
					found = 0;
				} else {
					bn = bn->next;
					block = (vfs_pipe_buffer_block_t*) bn->element;
				}
			}

			if(! found) { // create a new block
				block = malloc(sizeof(vfs_pipe_buffer_block_t*));
				block->base = malloc(PAGE_SIZE);
				block->length = 0;
				block->block_id = block_id =  info->num_blocks++;
				list_push_back(info->pipe_buffer, block);
				found = 1;
				block_off = 0;
				index = 0;
			}

			block->base[index++] = data[i];
		}

		send_event(info->event_id);
		info->event_id = get_new_event_ID();
		launch_pipe_handlers(info);
	} else {
		if (node->base == NULL) {
			node->base = malloc(node->length);
		} else {
			int pages_new = NUM_PAGES(node->length);
			int pages_old = NUM_PAGES(old_len);
			if(pages_new > pages_old)
				node->base = realloc(node->base, node->length);
		}

		uint8_t *nbase = (uint8_t*) node->base + off;
		uint8_t *wbase = (uint8_t*) base;
		memcpy(nbase, wbase, bytes);
	}

	return bytes;
}

/**
 * read the content from a vfs-node
 *
 * @param node the node, that will be readed
 * @param offset the offset from the node
 *
 * @return readed data
 */
void vfs_read(vfs_inode_t *node, uintptr_t offset, int len, void *buffer) {
	GET_INODE(node);
	if(node->length >= (offset+len)) {
		if(node->type == VFS_PIPE) {
			int block_id = offset / PAGE_SIZE;
			int block_off= offset % PAGE_SIZE;
			int end_block_id = (offset + len) / PAGE_SIZE;
			int end_block_off= (offset + len) % PAGE_SIZE;

			vfs_pipe_info_t *info = (vfs_pipe_info_t*) node->base;
			vfs_pipe_buffer_block_t *block = NULL;
			struct list_node *bn = info->pipe_buffer->head->next;

			int i;
			for(i = 0; i < info->num_blocks; i++) {
				block = (vfs_pipe_buffer_block_t*) bn->element;
				if(block->block_id == block_id) {
					if(block_id == end_block_id) {
						memcpy(buffer, block->base + block_off, end_block_off - block_off);
					} else {
						memcpy(buffer, block->base + block_off, PAGE_SIZE - block_off);
						block_off = 0;
						block_id ++;
					}
				}
				bn = bn->next;
			}
		} else {
			memcpy(buffer, node->base + offset, len);
		}
	}
}

/**
 * get stat of a node
 *
 * @param node node
 * @param buffer buffer
 *
 * @return _SUCCESS
 */
int vfs_stat(vfs_inode_t *node, struct stat *buffer) {
	uint8_t *node_stat = (uint8_t*) &node->stat;
	uint8_t *buf = (uint8_t*) buffer;
	int i = 0;
	while (i++ < sizeof(struct stat)) {
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
int vfs_access(vfs_inode_t *node, mode_t modus, uid_t uid, gid_t gid) {
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
vfs_inode_t *vfs_lookup_path(char *path) {
	vfs_inode_t *parent = root;

	if(path[0] != '/') {
		parent = current_thread->process->cwd;
	} else {
		path++;
	}

	int len = strlen(path);
	if(path[len-1] == '/') {
		path[len-1] = '\0';
	}

	char delimiter[] = "/";
	char *str = (char*) strtok(path, delimiter);
	while(str != NULL) {
		int num = parent->length / sizeof(vfs_dentry_t);
		vfs_dentry_t *entries = parent->base;
		int found = 0;
		int i;
		for(i = 0; i < num; i++) {
			if(strcmp(str, entries[i].inode->name)) {
				parent = entries[i].inode;
				found = 1;
			}
		}

		if(!found) {
			return NULL;
		} else {
			str = strtok(NULL, delimiter);
		}
	}

	return parent;
}

// Systemcalls

struct fd *get_fd(int fd) {
	struct fd *desc = NULL;
	struct list_node *node = current_thread->process->files->head->next;
	while(node != current_thread->process->files->head) {
		desc = node->element;
		if(desc->id == fd) {
			return desc;
		} else {
			node = node->next;
		}
	}

	return NULL;
}

void sys_open(struct cpu_state **cpu) {
	char *path = (char *) (*cpu)->CPU_ARG1;
	int oflags = (*cpu)->CPU_ARG2;
	mode_t mode = (*cpu)->CPU_ARG3;

	vfs_inode_t *inode = vfs_lookup_path(path);

	if(inode == NULL) {
		if(oflags & O_CREAT) { // create inode
			char *name = malloc(strlen((char*)path));// FIXME TODO
			strcpy(name, path);// FIXME TODO
			vfs_inode_t *parent = root; // FIXME TODO
			if(vfs_access(parent, W_OK, current_thread->process->uid, current_thread->process->gid == 0)) {
				inode = vfs_create_inode(name, mode, parent, current_thread->process->uid, current_thread->process->gid);
			} else {
				(*cpu)->CPU_ARG0 = _NO_PERMISSION;
				return;
			}
		} else {
			(*cpu)->CPU_ARG0 = _FAILURE;
			return;
		}
	} else {
		if(oflags & O_EXCL) {
			(*cpu)->CPU_ARG0 = _FAILURE;
			return;
		}
	}

	if(vfs_access(inode, R_OK, current_thread->process->uid, current_thread->process->gid == 0)) {
		if(oflags & O_TRUNC) {
			if(vfs_access(inode, W_OK, current_thread->process->uid, current_thread->process->gid == 0)) {
				memset(inode->base, 0, inode->length);
			} else {
				(*cpu)->CPU_ARG0 = _NO_PERMISSION;
				return;
			}
		}

		struct fd *desc = malloc(sizeof(struct fd));
		desc->id = list_length(current_thread->process->files);
		desc->mode = mode;
		desc->flags = oflags;
		desc->pos = 0;
		desc->inode = inode;

		list_push_back(current_thread->process->files, desc);

	        (*cpu)->CPU_ARG0 = desc->id;
	} else {
		(*cpu)->CPU_ARG0 = _NO_PERMISSION;
	}
}

void sys_pipe(struct cpu_state **cpu) {
	int *id = (int *) (*cpu)->CPU_ARG1;

	if(get_fd(id[0]) != NULL &&
	   get_fd(id[1]) != NULL)
	{
		vfs_inode_t *inode = vfs_create_pipe(current_thread->process->uid, current_thread->process->gid);

		// create read channel
		struct fd *desc0 = malloc(sizeof(struct fd));
		desc0->id = id[0];
		desc0->mode = 0x7ff;
		desc0->flags = O_RDONLY;
		desc0->pos = 0;
		desc0->inode = inode;
		list_push_back(current_thread->process->files, desc0);

		// create write channel
		struct fd *desc1 = malloc(sizeof(struct fd));
		desc1->id = id[1];
		desc1->mode = 0x7ff;
		desc1->flags = O_WRONLY;
		desc1->pos = 0;
		desc1->inode = inode;
		list_push_back(current_thread->process->files, desc1);

		(*cpu)->CPU_ARG0 = _SUCCESS;
	} else {
		(*cpu)->CPU_ARG0 = _FAILURE;
	}
}

void sys_close(struct cpu_state **cpu) {
	int fd = (*cpu)->CPU_ARG1;

	struct list_node *node = current_thread->process->files->head->next;
	int i;
	for(i = 0; i < list_length(current_thread->process->files); i++) {
		struct fd *desc = node->element;
		if(desc->id == fd) {
			list_remove_node(node);
			(*cpu)->CPU_ARG0 = _SUCCESS;
			return;
		} else {
			node = node->next;
		}
	}

	(*cpu)->CPU_ARG0 = _FAILURE;
}

void sys_read(struct cpu_state **cpu) {
	int fd = (*cpu)->CPU_ARG1;
	void *buf = (void*) (*cpu)->CPU_ARG2;
	size_t len = (*cpu)->CPU_ARG3;

	struct fd *desc = get_fd(fd);
	if(desc != NULL) {
		if(desc->flags & O_RDONLY ||
		   desc->flags & O_RDWR)
		{
			vfs_inode_t *inode = desc->inode;
			vfs_pipe_info_t *pipe = inode->base;

			if(vfs_access(inode, R_OK, current_thread->process->uid, current_thread->process->gid == 0)) {
				if(inode->length >= (desc->pos+len)) {
					vfs_read(inode, desc->pos, len, buf);
					desc->pos += len;
					(*cpu)->CPU_ARG0 = len;
				} else if(inode->type == VFS_PIPE) {
					add_trigger(WAIT_EVENT, pipe->event_id, 0, current_thread, sys_read);
					suspend_thread(current_thread);
					*cpu = (struct cpu_state *)task_schedule(*cpu);
				} else {
					(*cpu)->CPU_ARG0 = _FAILURE;
				}
			} else {
				(*cpu)->CPU_ARG0 = _NO_PERMISSION;
			}
		} else {
			(*cpu)->CPU_ARG0 = _NO_PERMISSION;
		}
	} else {
		(*cpu)->CPU_ARG0 = _FAILURE;
	}
}

void sys_write(struct cpu_state **cpu) {
	int fd = (*cpu)->CPU_ARG1;
	char *buf = (void*) (*cpu)->CPU_ARG2;
	size_t len = (*cpu)->CPU_ARG3;

    if(fd == 2) { // TODO FIXME
		int i;
		for(i = 0; i < len; i++) {
			printf("%c", buf[i]);
		}
		(*cpu)->CPU_ARG0 = _SUCCESS;
		return;
	}

	struct fd *desc = get_fd(fd);
	if(desc != NULL) {
		if(desc->flags & O_WRONLY ||
		   desc->flags & O_RDWR)
		{
			vfs_inode_t *inode = desc->inode;
			if(vfs_access(inode, W_OK, current_thread->process->uid, current_thread->process->gid == 0)) {
				if(! (desc->flags & O_APPEND) ) {
					desc->pos = 0;
					desc->flags |= O_APPEND;
				}

				int ret = vfs_write(inode, desc->pos, buf, len);
				(*cpu)->CPU_ARG0 = ret;
				if(ret > 0) {
					desc->pos += len;
				}
			} else {
				(*cpu)->CPU_ARG0 = _NO_PERMISSION;
			}
		} else {
			(*cpu)->CPU_ARG0 = _NO_PERMISSION;
		}
	} else {
		(*cpu)->CPU_ARG0 = _FAILURE;
	}
}

void sys_create(struct cpu_state **cpu) {
	char *name = (char *)(*cpu)->CPU_ARG1;
	int mode = (*cpu)->CPU_ARG2;

        // FIXME: only works in root
	vfs_inode_t *parent = root; // FIXME TODO
	if(parent != NULL) {
		if(vfs_access(parent, W_OK, current_thread->process->uid, current_thread->process->gid == 0)) {
			vfs_inode_t *inode = vfs_create_inode(name, mode, parent, current_thread->process->uid, current_thread->process->gid);

			if(inode != NULL) {
				struct fd *desc = malloc(sizeof(struct fd));
				desc->id = list_length(current_thread->process->files);
				desc->mode = mode;
				desc->flags = O_RDWR;
				desc->pos = 0;
				desc->inode = inode;

				list_push_back(current_thread->process->files, desc);

				(*cpu)->CPU_ARG0 = desc->id;
			} else {
				(*cpu)->CPU_ARG0 = _FAILURE;
			}
		} else {
			(*cpu)->CPU_ARG0 = _NO_PERMISSION;
		}
	} else {
		(*cpu)->CPU_ARG0 = _FAILURE;
	}
}

void sys_link(struct cpu_state **cpu) {
	char *src_path = (char *)(*cpu)->CPU_ARG1;
	char *dest_path = (char *)(*cpu)->CPU_ARG2;

	vfs_inode_t *src_inode = vfs_lookup_path(src_path);

	// FIXME: only works in root
	vfs_inode_t *dest_parent = root; // FIXME TODO

	if(src_inode != NULL && dest_parent != NULL) {
		if(vfs_access(src_inode, R_OK, current_thread->process->uid, current_thread->process->gid == 0) &&
		   vfs_access(dest_parent, W_OK, current_thread->process->uid, current_thread->process->gid == 0)) {
			vfs_inode_t *dest_inode = vfs_create_inode(dest_path, src_inode->stat.st_mode, dest_parent, current_thread->process->uid, current_thread->process->gid);

			if(dest_inode != NULL) {
				dest_inode->type = VFS_LINK;
				dest_inode->base = dest_inode;

				(*cpu)->CPU_ARG0 = _SUCCESS;
			} else {
				(*cpu)->CPU_ARG0 = _FAILURE;
			}
		} else {
			(*cpu)->CPU_ARG0 = _NO_PERMISSION;
		}
	} else {
		(*cpu)->CPU_ARG0 = _FAILURE;
	}
}

void sys_unlink(struct cpu_state **cpu) {
	char *path = (char *)(*cpu)->CPU_ARG1;

	vfs_inode_t *link = vfs_lookup_path(path);
	if(link != NULL) {
		if(vfs_access(link, W_OK, current_thread->process->uid, current_thread->process->gid == 0)) {
			link->base = NULL;
			link->type = VFS_REGULAR;
			free(link);
			(*cpu)->CPU_ARG0 = _SUCCESS;
		} else {
			(*cpu)->CPU_ARG0 = _NO_PERMISSION;
		}
	} else {
		(*cpu)->CPU_ARG0 = _FAILURE;
	}
}

void sys_chdir(struct cpu_state **cpu) {
	char *path = (char *)(*cpu)->CPU_ARG1;

	vfs_inode_t *nwd = vfs_lookup_path(path);
	if(nwd != NULL) {
		if(S_ISDIR(nwd->stat)) {
			if(vfs_access(nwd, R_OK, current_thread->process->uid, current_thread->process->gid == 0)) {
				current_thread->process->cwd = nwd;
				(*cpu)->CPU_ARG0 = _SUCCESS;
			} else {
				(*cpu)->CPU_ARG0 = _NO_PERMISSION;
			}
		} else {
			(*cpu)->CPU_ARG0 = _FAILURE;
		}
	} else {
		(*cpu)->CPU_ARG0 = _FAILURE;
	}
}

void sys_readdir(struct cpu_state **cpu) {
	static int pos = 0;
	static int old_fd = -1;
	int fd = (*cpu)->CPU_ARG1;
	//int count = (*cpu)->CPU_ARG2;		// count is currently unused, so i commented it out

	vfs_inode_t *parent = get_fd(fd)->inode;
	if(vfs_access(parent, R_OK, current_thread->process->uid, current_thread->process->gid == 0)) {
		dirent_t *dentry = (dirent_t *)(*cpu)->CPU_ARG2;

		vfs_dentry_t *entries = parent->base;
		int num = parent->length / sizeof(vfs_dentry_t);

		if(pos < num && (fd == old_fd || old_fd == -1)) {
			vfs_inode_t *ino = entries[pos++].inode;

			strcpy(dentry->name, ino->name);
			memcpy(&dentry->stat, &ino->stat, sizeof(struct stat));
			dentry->id = ino->stat.st_ino;
			(*cpu)->CPU_ARG0 = (uint32_t) dentry;
		} else {
			pos = 0;
			(*cpu)->CPU_ARG0 = (uint32_t) NULL;
		}

		old_fd = fd;
	} else {
		(*cpu)->CPU_ARG0 = _NO_PERMISSION;
	}
}

void sys_seek(struct cpu_state **cpu) {
	int fd = (*cpu)->CPU_ARG1;
	int off = (*cpu)->CPU_ARG2;
	int whence = (*cpu)->CPU_ARG3;
	
	struct fd *file = get_fd(fd);	
	
	file->flags |= O_APPEND;
	switch(whence) {
		case SEEK_SET: // absolute
			file->pos = off;
			break;
		case SEEK_CUR: // relative from current position
			file->pos += off;
			break;
		case SEEK_END: // relative from end
			file->pos = file->inode->length - off;
			break;
		default: // ???
			(*cpu)->CPU_ARG0 = _FAILURE;
			return;
	}
	
	(*cpu)->CPU_ARG0 = file->pos;
}

void sys_getcwd(struct cpu_state **cpu)
{
	char *buffer = (char *)(*cpu)->CPU_ARG1;
	int buffersize = (int)(*cpu)->CPU_ARG2;
	int currentend = 0;
	int cwdsize = 1;				// the final size of the final '\0'

	vfs_inode_t *inode = current_thread->process->cwd;

	if(inode == root)
		cwdsize++;

	while(inode != root)
	{
		cwdsize += strlen(inode->name);		// add the lenght of the directory name
		cwdsize++;				// add the length of the '/' between the directorys
		inode = inode->parent;			// next directory
	}

	if(buffer == NULL)
	{
		(*cpu)->CPU_ARG0 = 0;
		return;
		/*buffer = (char *)malloc(sizeof(char) * cwdsize);
		buffersize = cwdsize;*/					// TODO: make this working (some problems with kernel malloc)
	}

	if(buffersize < cwdsize)
	{
		(*cpu)->CPU_ARG0 = 0;
		return;
	}

	currentend = cwdsize - 1;		// because first element is buffer[0] (instead of buffer[1])
	buffer[currentend--] = '\0';	
	inode = current_thread->process->cwd;

	if(inode == root)
		buffer[currentend] = '/';

	while(inode != root)
	{
		currentend -= (strlen(inode->name) - 1);
		strcpy(&buffer[currentend--], inode->name);
		buffer[currentend--] = '/';
		inode = inode->parent;
	}

	(*cpu)->CPU_ARG0 =(unsigned int) buffer;
	return;
}

void launch_pipe_handlers(vfs_pipe_info_t *pipe) {
	struct list_node *node = pipe->handlers->head->next;
	struct list_node *head = pipe->handlers->head;

	while(node != head) {
		vfs_pipe_trigger_t *trigger = (vfs_pipe_trigger_t*) node->element;
		thread_create(current_thread->process, USERMODE, trigger->eip, NULL, trigger->argc, trigger->argv, NULL, NULL);

		node = node->next;
	}
}

void set_pipe_trigger(struct cpu_state **cpu){
	int fd = (*cpu)->CPU_ARG1;
	vfs_inode_t *inode = get_fd(fd)->inode;

	if(vfs_access(inode, R_OK, current_thread->process->uid, current_thread->process->gid == 0)) {
		vfs_pipe_info_t *pipe = inode->base;

		vfs_pipe_trigger_t *trigger = malloc(sizeof(vfs_pipe_trigger_t));
		trigger->eip = (*cpu)->CPU_ARG2;
		trigger->argc = (*cpu)->CPU_ARG3;
		trigger->argv = (void **)(*cpu)->CPU_ARG4;
		list_push_back(pipe->handlers, (void*) trigger);

		(*cpu)->CPU_ARG0 = _SUCCESS;
	} else {
		(*cpu)->CPU_ARG0 = _NO_PERMISSION;
	}
}


void sys_stat(struct cpu_state **cpu) {
	if((*cpu)->CPU_ARG2 == 0 || (*cpu)->CPU_ARG3 == 0){
		(*cpu)->CPU_ARG0 = -1;
		return;
	}

	vfs_inode_t *node = vfs_lookup_path((char*)(*cpu)->CPU_ARG2);
	if(node == NULL){
		(*cpu)->CPU_ARG0 = -1;
		return;
	}

	(*cpu)->CPU_ARG0 = vfs_stat(node,(struct stat *)(*cpu)->CPU_ARG3);
}


void sys_fstat(struct cpu_state **cpu)
{
	if((*cpu)->CPU_ARG2 == 0){
		(*cpu)->CPU_ARG0 = -1;
		return;
	}

	list_set_first(current_thread->process->files);
	while(!list_is_empty(current_thread->process->files) && !list_is_last(current_thread->process->files)){
		struct fd *file = list_get_current(current_thread->process->files);
		if(file->id == (*cpu)->CPU_ARG1) {
			(*cpu)->CPU_ARG0 = vfs_stat(file->inode, (struct stat *)(*cpu)->CPU_ARG2);
			return;
		}
		list_next(current_thread->process->files);
	}

	(*cpu)->CPU_ARG0 = -1;
}
