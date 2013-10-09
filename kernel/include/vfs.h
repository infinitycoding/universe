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
#include <cpu.h>

#define R_OK 0x1
#define W_OK 0x2
#define X_OK 0x4
#define F_OK 0x8

#define SEEK_SET 0x1
#define SEEK_CUR 0x2
#define SEEK_END 0x3

#define O_RDONLY 0x01 /* read only */
#define O_WRONLY 0x02 /* write only */
#define O_RDWR 0x04 /* read and write */
#define O_APPEND 0x08 /* append at end of file */
#define O_TRUNC 0x10 /* clear the file */
#define O_CREAT 0x20 /* create the file */
#define O_EXCL 0x40 /* forced file creation. if it already exist: fail */

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
vfs_inode_t *vfs_create_inode(char *name, mode_t mode, vfs_inode_t *parent);
vfs_dentry_t *vfs_create_dir_entry(vfs_inode_t *entry_inode);
int vfs_write(vfs_inode_t *inode, int off, void *base, int bytes);
void* vfs_read(vfs_inode_t *inode, uintptr_t offset);
int vfs_stat(vfs_inode_t *inode, struct stat *buffer);
int vfs_access(vfs_inode_t *inode, mode_t modus);
vfs_inode_t *vfs_lookup_path(char *path);

void open(struct cpu_state **cpu);
void pipe(struct cpu_state **cpu);
void close(struct cpu_state **cpu);
void read(struct cpu_state **cpu);
void write(struct cpu_state **cpu);

#endif
