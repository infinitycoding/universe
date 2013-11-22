#include <universe.h>
#include <math.h>

int thread(int argc, void **argv)
{
    print("you can't stop me!\n");
    return 0;
}

int main(int argc, char **argv) {
    print("Userspace!\n");
    //thread_launch(thread, 0, NULL);

    int pid = linux_syscall(SYS_FORK, 0, 0, 0, 0, 0);
    if(pid == 0) {
        print("Child!\n");
        exit(0);
    } else {
        print("Parent!\n");
        linux_syscall(SYS_WAITPID,-1,0,0,0,0);
        print("I'm back!\n");
    }

    int *a = alloc_memory(1);

    while(1);
    return 0;
}

