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

  #define VFS_DIR      0x1
  #define VFS_WRITABLE 0x2
  
  typedef struct vfs_node {
    char name[128];
    uint32_t node_id;
    uint8_t flags;
    void *base;
    uint32_t alloc;
    struct vfs_node *parent;
  } vfs_node_t;
  
  typedef struct vfs_dir_entry {
    uint32_t node_id;
    vfs_node_t *entry_node;
  } vfs_dir_entry_t;
  
  void INIT_VFS(void);
  vfs_node_t* vfs_create_node(char *name, uint8_t flags, vfs_node_t *parent);
  vfs_dir_entry_t* vfs_create_dir_entry(vfs_node_t *entry_node);
  int vfs_write(vfs_node_t *node, void *base, int bytes);
  void* vfs_read(vfs_node_t *node);
  int vfs_isFile(vfs_node_t *node);
  int vfs_isDir(vfs_node_t *node);
  
#endif
