#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <vfs_api.h>
#include <universe.h>

char file[] = "/ultrashell.elf";

int main(int argc, char **argv)
{
	linux_syscall(12, 5, 1, 0, 0, 0); // signal
	//linux_syscall(11, 5, 0, 0, 0, 0); // raise
	linux_syscall( 9, 0, 0, 0, 0, 0); // pause

	while(1);

    return 0;
}


