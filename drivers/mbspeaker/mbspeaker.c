/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the Universe Kernel.

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
 *  @file /drivers/mbspeaker/main.c
 *  @brief Driver for the motherboard speaker of x86 computers.
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */


// Libc includes
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// udrcp includes
#include <udrcp.h>
#include <udrcp/ioport.h>

port_t *speaker_switch;
port_t *count_register;
port_t *init_register;


void sound_on()
{
    outb(speaker_switch, inb(speaker_switch) | 3);
}

void sound_off()
{
    outb(speaker_switch, inb(speaker_switch) & ~3);
}

void sound(unsigned int frequency, unsigned int duration)
{
    uint16_t tmp;
    tmp = 1193180 / frequency;
    outb(init_register, 0xB6);
    outb(count_register, tmp & 0xFF);
    outb(count_register, tmp >> 8);
    sound_on();
    time_t t = time(NULL)+duration;
    while(time(NULL)<t);
    sound_off();
}



int main(void)
{
    int logfile = open("/var/log/driver/mbspeaker.log", O_WRONLY, 0);
    pckmgr *conn = new_pckmgr();
    if(!subsystem_connect(conn,HYPERVISOR,"udrcp",logfile,UHOST_DEFAULT_SYNCHRON))
    {
        udrcp_error(conn,"could not connect to host\n");
        return -1;
    }

    speaker_switch = port_alloc(conn,0x64);
    if(!speaker_switch)
    {
        udrcp_error(conn,"could not allocate port 0x64");
        return -2;
    }

    count_register = port_alloc(conn,0x42);
    if(!count_register)
    {
        udrcp_error(conn,"could not allocate port 0x42");
        return -2;
    }

    init_register  = port_alloc(conn,0x43);
    if(!init_register)
    {
        udrcp_error(conn,"could not allocate port 0x43");
        return -2;
    }
    
    int mret = mkfifo("/dev/mbspeaker", S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    int f = open("/dev/mbspeaker", O_WRONLY, 0);

    if(f < 0 || mret < 0)
    {
        udrcp_error(conn,"could not create fifo buffer /dev/mbspeaker\n");
          return -3;
    }


    while(1)
    {
          unsigned int note[] = {0,0};
          read(f, note, 2*sizeof(unsigned int));
          sound(note[0], note[1]);
    }

    return 0;
}