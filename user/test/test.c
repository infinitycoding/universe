#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <vfs_api.h>
#include <universe.h>

char file[] = "/ultrashell.elf";

int main(int argc, char **argv)
{
    int f = open("/dev/cga",O_WRONLY,0);
    char str[] = "Hello!\nWorld!";
    write(f,str,strlen(str));
	//printf("connecting to pid 2 on port 5..\n");
	//int sock = uconnect(2, "5");
	//printf("..connected .. %d\n", sock);

	//char str[19];
	//read(sock, &str, 19);
	//printf("read \"%s\"\n", str);

	return 0;
    /*linux_syscall(SYS_RENAME,(uint32_t) "foo.txt",(uint32_t) "bar.txt", 0, 0, 0);

    FILE *f = fopen("bar.txt", "r");
    char buffer[20];
    fread(buffer,20, 1, f);
    printf("%s\n",buffer);
    while(1);
*/
    bool success = true;
    success &= test_vfs_api();

    printf("================================================================================");
    if(success)
        printf("success\n");
    else
        printf("failed\n");
return 0;
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
		printf("child\n");
	}
	else
	{
		printf("parent\n");
	}

    return 0;
}


