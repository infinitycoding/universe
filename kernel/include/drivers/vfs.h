#ifndef _vfs_h_
#define _vfs_h_

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
#include <sys/stat.h>
#include <unistd.h>

typedef struct vfs_inode {
	char *name;
	void *base;
	uint32_t length;
	struct stat stat;
	struct vfs_inode *parent;
} vfs_inode_t;

typedef struct vfs_dentry {
	ino_t ino;
	vfs_inode_t *inode;
} vfs_dentry_t;

struct fd {
	unsigned int id;
	vfs_inode_t *inode;
	mode_t mode;
	int flags;
	int pos;
};

void INIT_VFS(void);
void set_vfs_uid(uid_t new_uid);
void set_vfs_gid(gid_t new_gid);
vfs_inode_t* vfs_create_inode(char *name, mode_t mode, vfs_inode_t *parent);
vfs_dentry_t* vfs_create_dir_entry(vfs_inode_t *entry_inode);
int vfs_write(vfs_inode_t *inode, void *base, int bytes);
void* vfs_read(vfs_inode_t *inode, uintptr_t offset);
int vfs_stat(vfs_inode_t *inode, struct stat *buffer);
int vfs_access(vfs_inode_t *inode, mode_t modus);
vfs_inode_t *vfs_lookup_path(char *path);

#endif
