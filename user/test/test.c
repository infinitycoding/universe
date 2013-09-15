#include <universe.h>

void _start(void) {
    print("Userspace!\nRead \"foo.txt\":\n");
    int fd = linux_syscall(SYS_OPEN, "foo.txt", 1, 0x1ff, 0, 0);
    char str[100] = "";
    linux_syscall(SYS_READ, fd, &str, 13, 0, 0);
    print(&str);
    
    exit(0);
}

