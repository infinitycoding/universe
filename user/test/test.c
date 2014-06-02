#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <vfs_api.h>
#include <universe.h>

int main(int argc, char **argv)
{
    bool success = true;
    success &= test_vfs_api();


    printf("================================================================================");
    if(success)
        printf("success\n");
    else
        printf("failed\n");

	int fd[2];
	linux_syscall(SYS_PIPE, (int)&fd, 0, 0, 0, 0);

	write(fd[1], "Test", 5);

	char buf[5];
	read(fd[0], &buf, 5);
	printf("%s\n", buf);

    return 0;
}


