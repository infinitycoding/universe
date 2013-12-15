/*
	Copyright 2012 universe coding group (UCG) all rights reserved
	This file is part of the Universe Kernel.

	Universe Kernel is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	Universe Kernel is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Universe Kernel.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <io.h>
#include <idt.h>
#include <printf.h>
#include <drivers/keyboard.h>
#include <drivers/keymap_de.h>
#include <vfs.h>
#include <thread.h>

extern struct thread_state *current_thread;

static bool shift = false;
static bool numlock = false;
static bool caps = false;

char keybuffer[512];
char* head = keybuffer;
char* tail = keybuffer;

vfs_inode_t *kbd_inode = NULL;

/**
 * Send a command to the Keyboard Controler
 *
 * @param port Port for the Command
 * @param command Command for the KBC
 *
 * @return void
 */
void send_kbc_command(uint8_t port, uint8_t command) {
  	while (inb(0x64) & 0x2);//Warten bis Eigabepuffer leer ist
  	outb(port, command);//KBC-Befehl senden
  	while (inb(0x60) != 0xFA);
}

/**
 * Send a command to the Keyboard
 *
 * @param command Command for the Keyboard
 *
 * @return void
 */
inline void send_kbd_command(uint8_t command) {
	send_kbc_command(0x60, command);
}

/**
 * Initalize the Keyboard
 *
 * @param void
 * @return void
 */
void INIT_KEYBOARD(void) {
	install_irq(0x1, &kbd_irq_handler);

  	while (!(inb(0x64) & 0x4));
  	// Puffer leeren
  	while (inb(0x64) & 0x1) {
  		inb(0x60);
  	}

  	send_kbd_command(0xF4);// Tastatur aktivieren
}

/**
 * IRQ handler for the Keyboard
 *
 * @param void
 * @return void
 */
void kbd_irq_handler(void) {
	uint8_t input = 0, ASCII = 0;
	while (inb(0x64)&1) {
		input = inb(0x60);

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

	if (ASCII) {
		if(kbd_inode != NULL) {
			vfs_pipe_info_t *info = kbd_inode->base;
			vfs_write(kbd_inode, info->length, &ASCII, 1);
		}
		
		*tail = ASCII;
		tail++;
		if (tail == keybuffer + 512) {
			tail = keybuffer;
		}
		if (head == keybuffer + 512) {
			head = keybuffer;
		}
		if (tail == head) {
			head++;
		}
	}

}


/**
 * get chars from the Keyboard buffer
 *
 * @param void
 * @return character
 **/
uint8_t input(void) {
	if (head == keybuffer+512) {
		head = keybuffer;
	}
	while (head == tail) {}
	return *(head++);
}


/**
 * set head to tail
 *
 * @param void
 * @return void
 **/
void seek_head(void) {
	head = tail;
}
