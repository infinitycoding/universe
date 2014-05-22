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



#include <vfs/module.h>
#include <pmm.h>
#include <mm/paging.h>
#include <string.h>
#include <mm/heap.h>



int map_all(struct multiboot_struct *mb_info)
{
     int i;
     struct mods_add *modules = (struct mods_add*) mb_info->mods_addr;

     for(i = 0; i < mb_info->mods_count; i++)
          map_module(&modules[i]);

     return i;
}


void map_module(struct mods_add *module)
{
     char *path;
     char *name;

     split_module_string(module->string, &path, &name);

     vfs_inode_t *node = vfs_create_path(path, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH | S_MODE_DIR, 0, 0);

     int size = module->mod_end - module->mod_start;
     int pages = NUM_PAGES(size);

     vaddr_t virtaddr = vmm_automap_kernel_range(current_context, module->mod_start, pages, VMM_WRITABLE);

     vfs_inode_t *file = vfs_create_inode(name,  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, node, 0, 0);

     file->base = (void *) virtaddr;
     file->length = size;
     file->stat.st_size = file->length;

     free(path);
     free(name);
}


void split_module_string(char *string, char **path_buffer, char **name_buffer)
{
     int i, j;

     for(i = 0, j = 0; string[i] != '\0'; i++)
          if(string[i] == '/')
               j++;

     for(i = 0; j > 0; i++)
          if(string[i] == '/')
               j--;

     (*path_buffer) = (char *)malloc(sizeof(char) * (i + 1));
     (*name_buffer) = (char *)malloc(sizeof(char) * ((strlen(string) - i) + 1));

     strncpy((*path_buffer), string, i);
     strncpy((*name_buffer), &string[i], strlen(string) - i);
}