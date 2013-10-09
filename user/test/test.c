#include <universe.h>
#include <math.h>

int thread(int argc, void **argv)
{
    print("you can't stop me!\n");
    return 0;
}

int main(int argc, char **argv) {
    print("Userspace!\nRead \"foo.txt\":\n");

    int fd[2] = {3, 4};

    // create a pipe
    linux_syscall(SYS_PIPE,(uint32_t) &fd, 0,0,0,0);

    //uint32_t OS = identify_os();


    thread_launch(thread, 0, NULL);


    asm volatile("_stop: jmp _stop");
    //exit(0);
    return 0;
}

