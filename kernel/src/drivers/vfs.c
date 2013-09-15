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
#include <heap.h>
#include <printf.h>
#include <unistd.h>
#include <drivers/vfs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread.h>
#include <memory_layout.h>
#include <paging.h>

extern struct thread_state *current_thread;

vfs_inode_t *root = NULL;
uint32_t nodes = 0;

static uid_t uid = 0;
static gid_t gid = 0;

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

	vfs_create_inode("foo.txt", 0x1ff, root);
}

/**
 * Set the User-ID
 *
 * @param new_uid the new user-id
 * @return void
 */
void set_vfs_uid(uid_t new_uid) {
	uid = new_uid;
}

/**
 * Set the Group-ID
 *
 * @param new_gid the new group-id
 * @return void
 */
void set_vfs_gid(gid_t new_gid) {
	gid = new_gid;
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
vfs_inode_t* vfs_create_inode(char *name, mode_t mode, vfs_inode_t *parent) {
	vfs_inode_t *node = malloc(sizeof(vfs_inode_t));
	node->name = name;
	node->length = 0;
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
	node->stat.st_atime = *get_time();
	node->stat.st_mtime = *get_time();

	return node;
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
	int i = 0;
	int writable = 0;
	if ((node->stat.st_uid == uid) &&
	    (node->stat.st_mode & S_IWUSR)) 
	{
		writable = 1;
	} 
	else if
	    ((node->stat.st_gid == gid) && 
	    (node->stat.st_mode & S_IWGRP))
	{
		writable = 1;
	} else {
		if (node->stat.st_mode & S_IWOTH)
			writable = 1;
	}

	if (writable) {
		int old_len = node->length;
    		if( (off + bytes) > node->length) {
			node->length = off + bytes;
			node->stat.st_size = node->length;
		}

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
	} else {
		printf("vfs: node %d isn't writable!\n", node->stat.st_ino);
	}
	return i;
}

/**
 * read the content from a vfs-node
 *
 * @param node the node, that will be readed
 * @param offset the offset from the node
 *
 * @return readed data
 */
void* vfs_read(vfs_inode_t *node, uintptr_t offset) {
	return (void*) node->base + offset;
}

/**
 * get stat of a node
 *
 * @param node node
 * @param buffer buffer
 *
 * @return success
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
int vfs_access(vfs_inode_t *node, mode_t modus) {
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
	vfs_inode_t *inode = NULL;
	
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
		vfs_dentry_t *entries = vfs_read(parent, 0);
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

void open(struct cpu_state **cpu) {
	char *path = (*cpu)->ebx;
	int oflags = (*cpu)->ecx;
	mode_t mode = (*cpu)->edx;

	vfs_inode_t *inode = vfs_lookup_path(path);

	if(inode == NULL) {
		if(oflags & O_CREAT) {// create inode
			char *name = malloc(strlen((char*)path));
			strcpy(name, path);
			inode = vfs_create_inode(name, mode, root);
		} else {
			(*cpu)->eax = -1;
			return;
		}
	} else {
		if(oflags & O_EXCL) {
			(*cpu)->eax = -2;
			return;
		}
	}
	
	if(oflags & O_TRUNC) {
		memset(inode->base, 0, inode->length);
	}
	
	struct fd *desc = malloc(sizeof(struct fd));
	desc->id = list_length(current_thread->process->files);
	desc->mode = mode;
	desc->flags = oflags;
	desc->pos = 0;	
	desc->inode = inode;

	list_push_back(current_thread->process->files, desc);

	(*cpu)->eax = desc->id;
}

void close(struct cpu_state **cpu) {

}

void read(struct cpu_state **cpu) {

}

void write(struct cpu_state **cpu) {

}


