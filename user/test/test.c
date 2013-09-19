#include <universe.h>

void _start(void) {
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

    exit(0);
}

