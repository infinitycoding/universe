#include <universe.h>
#include <stdio.h>
#include <udrcp.h>


int main(int argc, char **argv) {
    //printf("subsystem\n");
    //write(STDERR,"connection established",22);
    pckmgr *mgr = new_pckmgr(STDIN, STDOUT, STDERR);
    if(!subsystem_connect(mgr, UHOST_DEFAULT_ASYNCHRON))
        while(1);

    write(STDERR,"connection established",22);



    while(1){}
	return 0;
}

