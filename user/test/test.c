#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    printf("file append test: ");

    int f = 0;
    f = open("test",O_WRONLY,0);
    write(f, "+++++", 5);
    close(f);

    f = open("test",O_APPEND|O_WRONLY,0);
    write(f, "+++++", 5);
    close(f);


    char c[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    f = open("test",O_RDONLY,0);
    read(f,c,10);
    close(f);

    if(strncmp("++++++++++",c,15) == 0)
        printf("successful\n");
    else
        printf("failed\n");

    return 0;
}


