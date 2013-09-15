#include <universe.h>

void _start(void) {
    print("Userspace!\n");
    int fd = linux_syscall(SYS_OPEN, "foo.txt", 0, 0, 0, 0);
    char str[] = "0\n";
    str[0] += fd;
    print(&str);
    
    exit(0);
}

