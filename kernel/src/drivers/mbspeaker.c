/*-
	<Insert licence here :>>
 */
#include <io.h>

void sound_on() {
	outb(0x61, inb(0x61) | 3);
}

void sound_off() {
	outb(0x61, inb(0x61) & ~3);
}

void sound(uint32_t frequency, uint32_t time) {
	int i;
	uint16_t tmp;
	tmp = 1193180 / frequency;
	outb(0x43, 0xB6);
	outb(0x42, tmp & 0xFF);
	outb(0x42, tmp >> 8);
	sound_on();
	for (i = 0; i < time * 10000; i++);
	sound_off();
}
