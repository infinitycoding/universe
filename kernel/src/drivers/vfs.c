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



	 Diese Datei ist ein Teil vom Universe Kernel.

	 Das Universe Kernel ist Freie Software: Sie koennen es unter den Bedingungen
	 der GNU General Public License, wie von der Free Software Foundation,
	 Version 3 der Lizenz oder jeder sp‰teren
	 veroeffentlichten Version, weiterverbreiten und/oder modifizieren.

	 Das Universe Kernel wird in der Hoffnung, dass es nuetzlich sein wird, aber
	 Universe Kernel wird in der Hoffnung, dass es nuetzlich sein wird, aber
	 OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
	 Gew‰hrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN ZWECK.
	 Siehe die GNU General Public License fuer weitere Details.

	 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
	 Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/

/**
	@author Michael Sippel <micha.linuxfreak@gmail.com>
*/
#include <stdint.h>
#include <string.h>
#include <heap.h>
#include <printf.h>
#include <unistd.h>
#include <drivers/vfs.h>
#include <sys/stat.h>
#include <sys/types.h>

static vfs_node_t *root = NULL;
static uint32_t nodes = 0;

static uid_t uid = 0;
static gid_t gid = 0;

/**
 * Initalizing the Virtual File System
 *
 * @param void
 * @return void
 */
void INIT_VFS(void) {
  root = malloc(sizeof(vfs_node_t));
  root->stat.st_ino = 0;
  nodes = 1;
  root->alloc = 0;
  root->parent = NULL;
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
vfs_node_t* vfs_create_node(char *name, mode_t mode, vfs_node_t *parent) {
  vfs_node_t *node = malloc(sizeof(vfs_node_t));
  node->name = malloc(strlen(name));
  int i = 0;
  while (*name) {
	*node->name++ = *name++;
  }
  node->name[i] = (char) '\0';
  node->alloc = 0;
  node->base = NULL;
  if (parent == NULL) {
	node->parent = root;
  } else {
	node->parent = parent;
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
 * @param entry_node the node that will be entried
 * @return the new directory entry
 */
vfs_dir_entry_t* vfs_create_dir_entry(vfs_node_t *entry_node) {
  vfs_dir_entry_t *dir_ent = malloc(sizeof(vfs_dir_entry_t));
  
  dir_ent->ino = entry_node->stat.st_ino;
  dir_ent->entry_node = entry_node;
  
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
int vfs_write(vfs_node_t *node, void *base, int bytes) {
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
	if (node->base == NULL) {
	  node->base = malloc(bytes);
	} else {
	  node->base = realloc(node->base, node->alloc + bytes);
	}
	uint8_t *nbase = (uint8_t*) node->base + node->alloc;
	uint8_t *wbase = (uint8_t*) base;
	while (i++ < bytes) {
	  *nbase++ = *wbase++;
	  node->alloc++;
	}
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
void* vfs_read(vfs_node_t *node, uintptr_t offset) {
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
int vfs_stat(vfs_node_t *node, struct stat *buffer) {
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
int vfs_access(vfs_node_t *node, mode_t modus) {
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
