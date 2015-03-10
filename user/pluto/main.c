
#include <stdio.h>
#include <udrcp.h>
#include <driver.h>




int main(void)
{
    driver *kbd =  driverCreate("/drivers/keyboard.elf");
    driver *cga =  driverCreate("/drivers/cga.elf");
    
    driverLaunch(cga,1);
    driverLaunch(kbd,1);
    
    list_t *barlist = list_create();
    
    crossbarAdd(barlist, kbd, cga);
    crossbarLaunch(barlist);
  
    return 0;
}

