#include <udrcp.h>
#include <drivers/hosts/uhost.h>
#include <scheduler.h>
#include <list.h>

list_t *drivers;

void INIT_UHOST(void)
{
    drivers = list_create();


}


void kill_subdriver()
{

}





