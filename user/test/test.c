#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <vfs_api.h>
#include <universe.h>

char file[] = "/ultrashell.elf";

int main(int argc, char **argv)
{
    /*linux_syscall(SYS_RENAME,(uint32_t) "foo.txt",(uint32_t) "bar.txt", 0, 0, 0);

    FILE *f = fopen("bar.txt", "r");
    char buffer[20];
    fread(buffer,20, 1, f);
    printf("%s\n",buffer);
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


	int pid = fork();
	printf("hi ");
	if(pid == 0)
	{
		printf("child.. executing ultrashell.\n");
		execve("/test.elf", NULL, NULL);
		printf("das soll nicht kommen.\n");
	}
	else
	{
		printf("parent\n");
	}*/

	printf("hi all!\n");

	int err = execve("/ultrashell.elf", NULL, NULL);
	printf("error %d\n", err);

    return 0;
}


