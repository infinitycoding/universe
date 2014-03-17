#include <universe.h>
#include <stdio.h>
#include <math.h>

void kbd_thread(void) {
  printf("X");
  thread_exit(0);
}

int main(int argc, char **argv) {
    printf("subsystem\n");

    universe_syscall(SYS_PIPE_TRIGGER, stdin, (uint32_t)&kbd_thread,0,0,0);

    while(1){}
	return 0;
}

