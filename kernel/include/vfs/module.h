#ifndef MODULE_H_INCLUDED
#define MOULDE_H_INCLUDED

/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the Universe Kernel.

     The Universe Kernel is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.

     The Universe Kernel is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with the Universe Kernel. If not, see <http://www.gnu.org/licenses/>.
 */



#include <sys/multiboot.h>
#include <vfs/vfs.h>


typedef enum successful       // optical reasons
{
     success, failed
}success_t;

struct mapping_statistics     // how many modules were loaded and how many failed
{
     int total;
     int load_success;
     int load_failed;
};


struct mapping_statistics map_all(struct multiboot_struct *mb_info);
success_t map_module(struct mods_add *module);
success_t split_module_string(char *string, char **path_buffer, char **name_buffer);

#endif