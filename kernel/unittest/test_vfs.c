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

/**
 *  @author Michael Sippel <micha@infinitycoding.com>
 */

#include <vfs/vfs.h>
#include <printf.h>
#include <mm/heap.h>


#define COUNT 100
#define STEP (int)((float)COUNT/(float)20)
extern vfs_inode_t *root;

bool test_vfs()
{
    int i,j;
    char name[256];
    vfs_inode_t **inodes = malloc(sizeof(vfs_inode_t*) * COUNT);

    printf("creating %d inodes...\n", COUNT);
    for(i = 0; i < COUNT; i++)
    {
        sprintf(name, "test_inode%d", i);
        inodes[i] = vfs_create_inode(name, S_IRUSR | S_IWUSR, root, 0, 0);

        if((i % STEP) == 0)
        {
            printf("=");
        }
    }
    printf("\n");

    printf("writing a lot in them...\n");
    for(i = 0; i < COUNT; i++)
    {
        char *buffer = malloc(10000);
        for(j = 0; j < 10000; j++)
        {
            buffer[j] = 'a' + j%25;
        }

        vfs_write(inodes[i], 1, 0, buffer, 10000);

        free(buffer);

        if((i % STEP) == 0)
        {
            printf("=");
        }
    }
    printf("\n");

    printf("read them out and check!\n");
    int fails=0;
    for(i = 0; i < COUNT; i++)
    {
        char *buffer = malloc(10000);
        vfs_read(inodes[i], 0, 0, buffer, 10000);

        for(j = 0; j < 10000; j++)
        {
            char testchar = 'a' + j%25;
            if(buffer[j] != testchar)
            {
                printf("failed at %d\n", j);
                fails ++;
                break;
            }
        }

        free(buffer);

        if((i % STEP) == 0)
        {
            printf("=");
        }
    }
    printf("\n");

    printf("%d of %d inodes failed\n", fails, COUNT);

    free(inodes);
    if(fails)
        return false;
    return true;
}

