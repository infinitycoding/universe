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
	uint8_t c = inb(0x60);
	//putchar(c);
}



