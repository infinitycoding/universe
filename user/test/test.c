#include <universe.h>
#include <math.h>

int thread(int argc, void **argv)
{
    print("you can't stop me!\n");
    return 0;
}

int main(int argc, void **argv) {
    print("Userspace!\nRead \"foo.txt\":\n");

    int fd[2] = {3, 4};

    // create a pipe
    linux_syscall(SYS_PIPE, &fd, 0,0,0,0);
    
    uint32_t OS = identify_os();

    if(OS is UNIVERSE_OS)
        print("OS: Universe OS\n");

    thread_launch(thread, 0, NULL);
    int n = abs(-4);

    asm volatile("_stop: jmp _stop");
    //exit(0);
}

