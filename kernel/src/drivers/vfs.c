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
#include <printf.h>
#include <drivers/vfs.h>

static vfs_node_t *root = NULL;
static uint32_t nodes = 0;
/**
 * Initalizing the Virtual File System
 *
 * @param void
 * @return void
 */
void INIT_VFS(void) {
  root = malloc(sizeof(vfs_node_t));
  root->node_id = 0;
  nodes = 1;
  root->flags = VFS_DIR | VFS_WRITABLE;
  root->alloc = 0;
  root->parent = NULL;
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
vfs_node_t* vfs_create_node(char *name, uint8_t flags, vfs_node_t *parent) {
  vfs_node_t *node = malloc(sizeof(vfs_node_t));
  int i = 0;
  while(*name && i < 128) {
    node->name[i++] = *name++;
  }
  node->name[i] = (char) '\0';
  node->node_id = nodes++;
  node->flags = flags;
  node->alloc = 0;
  node->base = NULL;
  if(parent == NULL) {
    node->parent = root;
  } else {
    node->parent = parent;
  }
  
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
  
  dir_ent->node_id = entry_node->node_id;
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
  if(node->flags & VFS_WRITABLE) {
    if(node->base == NULL) {
      node->base = malloc(bytes);
    } else {
      node->base = realloc(node->base, node->alloc + bytes);
    }
    uint8_t *nbase = (uint8_t*) node->base + node->alloc;
    uint8_t *wbase = (uint8_t*) base;
    while(i++ < bytes) {
      *nbase++ = *wbase++;
      node->alloc++;
    }
  } else {
    printf("vfs: node %d isn't writable!\n", node->node_id);
  }
  return i;
}

/**
 * read the content from a vfs-node
 *
 * @param node the node, that will be readed
 * @return readed data
 */
void* vfs_read(vfs_node_t *node) {
  return (void*) node->base;
}

/**
 * check, if node is a file
 *
 * @param node node to check
 * @return if node is a file: 0
 *         if node is a directory: 1
 */
int vfs_isFile(vfs_node_t *node) {
  return (node->flags & VFS_DIR) ? 0 : 1;
}

/**
 * check, if node is a directory
 *
 * @param node node th check
 * @return if node is a directory: 0
 *         if node is a file: 1
 */
int vfs_isDir(vfs_node_t *node) {
  return (node->flags & VFS_DIR) ? 1 : 0;
}

