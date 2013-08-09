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
	@author Michael Sippel <micha.linuxfreak@gmail.com>
*/

#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct vfs_node {
	char *name;
	struct stat stat;
	void *base;
	uint32_t alloc;
	struct vfs_node *parent;
} vfs_node_t;

typedef struct vfs_dir_entry {
	ino_t ino;
	vfs_node_t *entry_node;
} vfs_dir_entry_t;

void INIT_VFS(void);
void set_vfs_uid(uid_t new_uid);
void set_vfs_gid(gid_t new_gid);
vfs_node_t* vfs_create_node(char *name, mode_t mode, vfs_node_t *parent);
vfs_dir_entry_t* vfs_create_dir_entry(vfs_node_t *entry_node);
int vfs_write(vfs_node_t *node, void *base, int bytes);
void* vfs_read(vfs_node_t *node, uintptr_t offset);
int vfs_stat(vfs_node_t *node, struct stat *buffer);
int vfs_access(vfs_node_t *node, mode_t modus);

#endif
