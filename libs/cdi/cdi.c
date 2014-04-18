/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the CDI-wrapper-library.
 
     The CDI-wrapper-library is free software: you can redistribute it and/or modify
     it under the terms of the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.
 
     The CDI-wrapper-library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser General Public License for more details.
 
     You should have received a copy of the GNU Lesser General Public License
     along with the CDI-wrapper-library.  If not, see <http://www.gnu.org/licenses/>.
 */



/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 *  based on the CDI reference implementation by Kevin Wolf
 */

#include <udrcp.h>
#include <cdi.h>

/** standart connection manager for CDI drivers*/
pckmgr *conn;

/**
 * initiates a udrcp connection for the cdi device
 */
 void cdi_init(void)
{
    conn = new_pckmgr(stdin,stdout,stderr);
    if(!subsystem_connect(conn, UHOST_DEFAULT_SYNCHRON))
    {
          udrcp_error(conn,"could not connect to host\n");
          exit(-2);
    }

    /*
     read the driver list and initiate them.
    */

}

/**
 * Weak main function to perform the driver as standalone UDRCP-capable driver.
 * @param argc Number of shell parameters
 * @param argv shell parameters
 * @return if success 0 otherwise an errorcode
 */
int __attribute__((weak))main(int argc, char *argv[])
{
     cdi_init();
     while(1); // to do: poll packages
     return 0;
}

/**
 * Initiates the cdi driver structure.
 * @param driver Pointer to the driver structure
 */
void cdi_driver_init(struct cdi_driver* driver)
{
    driver->devices = cdi_list_create();
}

/**
 * Destroys the cdi driver structure.
 * @param driver Pointer to the driver structure
 */
void cdi_driver_destroy(struct cdi_driver* driver)
{
     cdi_list_destroy(driver->devices);
}