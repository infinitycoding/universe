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
#include <vfs/vfs.h>
#include <pmm.h>
#include <mm/paging.h>
#include <string.h>
#include <mm/heap.h>



/**
 * @brief maps all multiboot modules into the vfs
 * @param mb_info the multiboot struct with the modules in it
 * @return how many modules got mapped and how many failed
 */
struct mapping_statistics map_all(struct multiboot_struct *mb_info)
{
    struct mapping_statistics s;
    memset(&s, 0, sizeof(struct mapping_statistics));
    struct mods_add *modules = (struct mods_add*) mb_info->mods_addr;

    for(s.total = 0; s.total < mb_info->mods_count; s.total++)
    {
        if(map_module(&modules[s.total]) == success)
            s.load_success++;
        else
            s.load_failed++;
    }

    return s;
}


/**
 * @brief maps one module into the vfs
 * @param module the information i need to map this module
 * @return success or failture
 */
success_t map_module(struct mods_add *module)
{
    if(module == NULL)  // i don't think this will ever happen, but you have what you have
        return failed;

    char *path = NULL;  // later i will need path and name
    char *name = NULL;

    if(split_module_string(module->string, &path, &name) == failed)     // if i can't split the module string there is no reason for executing further
        return failed;

    printf("%s%s\n", path, name);

    vfs_inode_t *node = vfs_create_path(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_MODE_DIR, 0, 0);  // i need a node to place the module in

    int size = module->mod_end - module->mod_start;     // basic stuff

    if(size < 0)        // should never happen, but you never know
    {
        free(path);     // i have to free the allocated memory (to avoid memory leaks)
        free(name);
        return failed;
    }

    int pages = NUM_PAGES(size);

    vaddr_t virtaddr = vmm_automap_kernel_range(current_context, module->mod_start, pages, VMM_WRITABLE);   // something about physical and virtual memory and kernel and user space... ask someone else

    vfs_inode_t *file = vfs_create_inode(name,  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH, node, 0, 0);   // create the module in the vfs
	vfs_write(file, 0, virtaddr, size);

    free(path);     // "we don't waste memory" (tdotu)
    free(name);

    return success; // be happy
}


/**
 * @brief splits a module string (containing path and name) in path and name.
 * @param string the module string
 * @param path_buffer the adress of the char pointer in which the path should be placed
 * @param name_buffer the adress of the char pointer in which the name should be placed
 * @return success or failture
 */
success_t split_module_string(char *string, char **path_buffer, char **name_buffer)
{
    if(string == NULL || path_buffer == NULL || name_buffer == NULL)    // i don't think this will ever happen, but you have what you have
        return failed;

    int i, j;

    for(i = 0, j = 0; string[i] != '\0'; i++)       // in this loop i count the number of slashes in a string
        if(string[i] == '/')
            j++;

    if(j == 0)  // illegal path
        return failed;

    for(i = 0; j > 0; i++)                          // in this loop i count the number of characters from the beginning to the final slash
        if(string[i] == '/')
            j--;

    (*path_buffer) = (char *)malloc(sizeof(char) * (i + 1));    // the length of the path is the number of characters to the final slash plus one for zero-termination

    if((*path_buffer) == NULL)  // if malloc fails (virtually impossible)
        return failed;

    memset((*path_buffer), '\0', sizeof(char) * (i + 1));

    (*name_buffer) = (char *)malloc(sizeof(char) * ((strlen(string) - i) + 1));     // the length of the name is the number of characters after the final slash plus one for zero-termination

    if((*name_buffer) == NULL)  // if malloc fails (virtually impossible)
    {
        free(*path_buffer);     // i have to free the successfully allocated memory (to avoid memory leaks)
        return failed;
    }

    memset((*name_buffer), '\0', sizeof(char) * ((strlen(string) - i) + 1));

    strncpy((*path_buffer), string, i);                         // copy the two parts of the string in the two other strings
    strncpy((*name_buffer), &string[i], strlen(string) - i);

    return success;
}
