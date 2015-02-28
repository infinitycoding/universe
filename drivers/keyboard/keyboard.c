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
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

// Libc includes
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// udrcp includes
#include <udrcp.h>
#include <udrcp/ioport.h>

// modules includes
#include <keymap_de.h>

static bool shift = false;
static bool numlock = false;
static bool caps = false;
port_t *kbc_stat;
port_t *kbc_io;
int f;
void kbd_irq_handler(void);

/**
 * Initalize the Keyboard
 *STDOUT_FILENO
 * @param void
 * @return void
 */
int main(void)
{
    int logfile = open("/var/log/drivers/keyboard.log", O_WRONLY, 0);
    
    pckmgr *conn = new_pckmgr();
    if(!subsystem_connect(conn,HYPERVISOR,"udrcp",logfile,UHOST_DEFAULT_SYNCHRON))
    {
        printf("XYZ\n");
        udrcp_error(conn,"could not connect to host\n");
        return -1;
    }
    udrcp_error(conn," /dev/kbd created\n");

    kbc_stat = port_alloc(conn,0x64);
    kbc_io = port_alloc(conn,0x60);

    if(!kbc_io || !kbc_stat)
    {
        udrcp_error(conn,"could not allocate port");
        return -2;
    }

    while (!(inb(kbc_stat) & 0x4));
  	// Puffer leeren
  	while (inb(kbc_stat) & 0x1)
  		inb(kbc_io);

  	while (inb(kbc_stat) & 0x2);
  	outb(kbc_io, 0xF4);
  	while (inb(kbc_io) != 0xFA);


    int mret = mkfifo("/dev/kbd", S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    f = open("/dev/kbd", O_WRONLY, 0);

    if(f < 0 || mret < 0)
    {
        udrcp_error(conn,"could not create fifo buffer /dev/kbd\n");
		    return 1;
    }
    if(!req_intsig(conn, 0x1))
    {
        udrcp_error(conn,"could not get interrupt signal for IRQ 0x1\n");
        return 1;
    }

    while(1)
    {
        pck_t *signal = poll_next(conn);
        switch(signal->type)
        {
            case INTSIG:
                kbd_irq_handler();
            break;

            case RESET_CON:
                reset_conn(conn);
            break;

            case SHUTDOWN:
                printf("shutdown!\n");
                while(1);
            break;

            case RESTART:
            case CHKDEV:
                udrcp_error(conn,"unusual signal\n");
            break;

            default:
                udrcp_error(conn,"unknown signal\n");
            break;
        }
        free_pck(signal);
    }

    return 0;
}

/**
 * IRQ handler for the Keyboard
 *
 * @param void
 * @return void
 */
void kbd_irq_handler(void)
{
	uint8_t input = 0, ASCII = 0;
	while (inb(kbc_stat)&1) {
		input = inb(kbc_io);

		if (input==0xC5) { //pressed numlock
			numlock = !numlock;
			continue;
		}

		if (input==0x3A) { //pressed capslock
			caps = !caps;
			continue;
		}


		if ((input & 0x80) == 0x80) { //release Key
		    input &= 0x7F;

		    if (input == 0x2A || input == 0x36) { //released shift key
		        shift = false;
		    }
			continue;
		}

		if (input == 0x2A || input == 0x36) { //pressed shift
			shift = true;
			continue;
		}

		if (input > 0x46 && input < 0x55 && input != 0x4A && input != 0x4C && input != 0x4E && numlock) { //Numblock Key
			ASCII = asciishift[input];
		}

		else if (shift || caps) { //Common Key
			ASCII = asciishift[input];
		} else {
			ASCII = asciinormal[input];
		}

	}

	if (ASCII)
  {
        write(f, &ASCII, sizeof(char));
  }

}
