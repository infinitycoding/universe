#ifndef _uhost_h_
#define _uhost_h_
/*
    Copyright 2013 universe coding group (UCG) all rights reserved
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
#include <vfs.h>
#include <multiboot.h>

void INIT_UHOST(int argc, void **argv);

struct uhost_driver
{
    uint32_t last_packageID;
    vfs_inode_t *stdin;
    vfs_inode_t *stdout;
    vfs_inode_t *stderr;
};
#endif
