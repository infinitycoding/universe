#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <vfs_api.h>

int main(int argc, char **argv)
{
    bool success = true;
    success &= test_vfs_api();

    printf("=========================================================================\n");
    if(success)
        printf("success\n");
    else
        printf("failed\n");

    return 0;
}


