/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the universe CGA-Driver.
 
     The Universe Kernel is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.
 
     The Universe Kernel is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
 
     You should have received a copy of the GNU General Public License
     along with the Universe Kernel. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <udrcp.h>
#include <udrcp/ioport.h>
#include <udrcp/memory.h>
#include <cga.h>
#include <crtc.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    /*! driver internal variables*/
    color_t color = WHITE | BLACK << 4;



    pckmgr *conn = new_pckmgr();
    int logfile = open("/var/log/driver/cga.log", O_WRONLY, 0);
    
    if(!subsystem_connect(conn,HYPERVISOR,"udrcp",logfile,UHOST_DEFAULT_SYNCHRON))
    {
        udrcp_error(conn,"could not connect to host\n");
        return -1;
    }

    // todo: parser command line imput to get the terminal resolution
    pma_mem_t *videomem = alloc_pma_area(conn, 0xb8000, 4096);
    struct cga_handle *cga = malloc(sizeof(struct cga_handle));
    cga->lines = 25;
    cga->cols = 80;
    cga->xpos = 0;
    cga->ypos = 0;
    cga->video_mem = videomem->mem_base;

    // CRTC stuff for courser handling
    struct crtc_handle *crtc = malloc(sizeof(struct crtc_handle));
    crtc->index = port_alloc(conn, CRTC_INDEX_PORT);
    crtc->data = port_alloc(conn, CRTC_DATA_PORT);
    if(!crtc->index || !crtc->data)
        udrcp_error(conn,"Warning: no CRTC available\n");
    
    mkfifo("/dev/cga", S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    
    int input = open("/dev/cga",O_RDONLY,0);
    
    while(1)
    {
      char inchar;
      read(input, &inchar, sizeof(char));
      putchar(crtc,cga, inchar,color);
        
    }
    
    


    //todo: read input from file
    // cause of not implementation: vfs has some weird bugs. I'll wait untill micha fixed that
    return 0;
}
