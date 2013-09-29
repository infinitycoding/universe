#include <universe.h>

int thread(int argc, void **argv)
{
    print("you can't stop me!\n");
    return 0;
}

void main(int argc, void **argv) {
    print("Userspace!\nRead \"foo.txt\":\n");

    // open file
    int fd = linux_syscall(SYS_OPEN, "foo.txt", 4, 0x1ff, 0, 0);
    char str[12];

    // read
    linux_syscall(SYS_READ, fd, &str, 12, 0, 0);
    print(&str);

    // write
    char str2[] = "New text!";
    linux_syscall(SYS_WRITE, fd, &str, 10, 0, 0);

    // close file
    linux_syscall(SYS_CLOSE, fd, 0, 0, 0, 0);

        uint32_t OS = identify_os();

    if(OS is UNIVERSE_OS)
        print("OS: Universe OS\n");

    thread_launch(thread, 0, NULL);
    //linux_syscall(SYS_FORK, 0, 0, 0, 0, 0);

    asm volatile("_stop: jmp _stop");
    //exit(0);
}

