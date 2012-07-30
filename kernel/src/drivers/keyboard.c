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



    Diese Datei ist ein Teil vom Universe Kernel.

    Das Universe Kernel ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder jeder späteren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Das Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEWÄHELEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License für weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <io.h>
#include <idt.h>
#include <printf.h>
#include <drivers/keyboard.h>
#include <drivers/keymap_de.h>

static uint8_t shifted = 0;
static uint8_t numlocked = 0;
static uint8_t lastscode=0;

char keybuffer[512];
char* head=keybuffer;
char* tail=keybuffer;

/**
 * Send a command to the Keyboard Controler
 *
 * @param port Port for the Command
 * @param command Command for the KBC
 *
 * @return void
 */
void send_kbc_command(uint8_t port, uint8_t command){
  	while(inb(0x64) & 0x2);//Warten bis Eigabepuffer leer ist
  	outb(port, command);//KBC-Befehl senden
  	while(inb(0x60) != 0xFA);
}

/**
 * Send a command to the Keyboard
 *
 * @param command Command for the Keyboard
 *
 * @return void
 */
inline void send_kbd_command(uint8_t command){
	send_kbc_command(0x60, command);
}

/**
 * Initalize the Keyboard
 *
 * @param void
 * @return void
 */
void INIT_KEYBOARD(void){
	install_irq(0x1, &kbd_irq_handler);

  	while(!(inb(0x64) & 0x4));
  	// Puffer leeren
  	while (inb(0x64) & 0x1){
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
void kbd_irq_handler(void){
	uint8_t input=0,ASCII=0;
	while (inb(0x64)&1){
		input=inb(0x60);
		if((input & 0x80) != 0x80){
            if(input==0xC5){
                numlocked = !numlocked;
            }
            else if(input==0x2A || input==0x36){
                    shifted=1;
            }
            else if(input==0xAA || input==0xB6){
                    shifted=0;
            }
            else if(input == 0x49 || input == 0x47 || input == 0x4B || input == 0x51 || input == 0x48 || input == 0x4F || input == 0x50 || input == 0x52 || input == 0x53 || input == 0x4D){
                if(numlocked == 0){
                    ASCII=asciinormal[input];
                }
                else if(numlocked == 1){
                    ASCII=asciishift[input];
                }
            }
            else{
                if(shifted==0 ){
                    ASCII=asciinormal[input];
                }else{
                    ASCII=asciishift[input];
                }
            }
		}
	}

    *tail=ASCII;
    tail++;
    if (tail==keybuffer+512) {
        tail=keybuffer;
    }
	if (head==keybuffer+512) {
		head=keybuffer;
	}
	if (tail==head) {
		head++;
	}

}


/**
 * get chars from the Keyboard buffer
 *
 * @param void
 * @return character
 **/
uint8_t input(void) {
	if (head==keybuffer+512) {
		head=keybuffer;
	}
	while(head==tail){}
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
