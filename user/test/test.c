#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <vfs_api.h>
#include <universe.h>

int main(int argc, char **argv)
{
	while(1);
    bool success = true;
    success &= test_vfs_api();

    printf("================================================================================");
    if(success)
        printf("success\n");
    else
        printf("failed\n");

	while(1);

	int fd[2];
	linux_syscall(SYS_PIPE, (int)&fd, 0, 0, 0, 0);

	char buf[10];

	write(fd[1], "hallo", 6);
	write(fd[1], "von", 4);

	read(fd[0], &buf, 6);
	printf("%s\n", buf);

	read(fd[0], &buf, 4);
	printf("%s\n", buf);

	write(fd[1], "test", 5);

	read(fd[0], &buf, 5);
	printf("%s\n", buf);

    return 0;
}


