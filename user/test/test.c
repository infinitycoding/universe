#include <universe.h>
#include <math.h>

int thread(int argc, void **argv)
{
    print("you can't stop me!\n");
    return 0;
}

int main(int argc, char **argv) {
    print("Userspace!\n");
    thread_launch(thread, 0, NULL);

    while(1);
    
    return 0;
}

