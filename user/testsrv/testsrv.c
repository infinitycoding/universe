#include <universe.h>

int main(int argc, char **argv)
{
	linux_syscall(10, 3, 5, 0, 0, 0); // send signal 5 to pid 1
	while(1);

	/*printf("started testserver with pid %d\n", getpid());

	int shm_id = shmget(1234, 0x4000, 0x1);
	printf("testsrv: created shared memory segment: %d\n", shm_id);

	char *addr = shmat(shm_id, 0, 0);
	printf("testsrv: attached shared memory at %x\n", addr);

	while(1)
	{
		if(addr[0] != '\0')
		{
			printf("testsrv: read from shm: \"%s\"\n", addr);
			while(1);
		}
	}
*/
	/*
	open_port("5");

	while(1)
	{
		int req_id = uread_port("5");

		printf("gotten request %d\n", req_id);
		int fd = uaccept(req_id);
		printf("accepted. (%d)\n", fd);

		write(fd, "Hello from testsrv", 19);
	}*/
}

