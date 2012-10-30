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

/**
	@author Tom Slawik <tom.slawik@gmail.com>
	@author Simon Diepold <simon.diepold@infinitycoding.de>
*/

#include <idt.h>
#include <cpu.h>
#include <printf.h>

void halt(void) {
	asm volatile("cli");
	while (1) {}
}

char *exception_messages[] =
{
	"Division By Zero",        "Debug",                         "Non Maskable Interrupt",    "Breakpoint",
	"Into Detected Overflow",  "Out of Bounds",                 "Invalid Opcode",            "No Coprocessor",
	"Double Fault",            "Coprocessor Segment Overrun",   "Bad TSS",                   "Segment Not Present",
	"Stack Fault",             "General Protection Fault",      "Page Fault",                "Unknown Interrupt",
	"Coprocessor Fault",       "Alignment Check",               "Machine Check",             "SIMD Floating Point",
	"Reserved",                "Reserved",                      "Reserved",                  "Reserved",
	"Reserved",                "Reserved",                      "Reserved",                  "Reserved",
	"Reserved",                "Reserved",                      "Reserved",                  "Reserved"
};

int cpu_dump(struct cpu_state* cpu, char *str)
{
	int len = 0;

	len += sprintf(str + len, "EAX:  %#010X    EBX:     %#010X\n",  cpu->eax, 	cpu->ebx);
	len += sprintf(str + len, "ECX:  %#010X    EDX:     %#010X\n",  cpu->ecx, 	cpu->edx);
	len += sprintf(str + len, "ESI:  %#010X    EDI:     %#010X\n",  cpu->esi, 	cpu->edi);
	len += sprintf(str + len, "ESP:  %#010X    EBP:     %#010X\n",  cpu->esp, 	cpu->ebp);
	len += sprintf(str + len, "CS:   %#010X    DS:      %#010X\n",  cpu->cs, 	cpu->ds);
	len += sprintf(str + len, "SS:   %#010X    ES:      %#010X\n",  cpu->ss, 	cpu->es);
	len += sprintf(str + len, "GS:   %#010X    FS:      %#010X\n",  cpu->gs, 	cpu->fs);
	len += sprintf(str + len, "EIP:  %#010X    EFLAGS:  %#010X\n",  cpu->eip, 	cpu->eflags);

	return len;
}


// just used in case of untreated exceptions
void exc_panic(struct cpu_state* cpu)
{
	char message[512];
	char *exception = exception_messages[cpu->intr];
	int len = 0;

	len = sprintf(message, "%s\n\n", exception, cpu->error);
	cpu_dump(cpu, message + len);

	panic(message);
}

void panic(char *message)
{
	char buffer[1024];
	int lines = 8;
	int len = 0;

	len += sprintf(buffer + len,
		"      |==================================================================|      \n"
		"      |                             Universe                             |      \n"
		"      |==================================================================|      \n"
		"\n"
		"\n"
		"      Universe has been crashed. You have to restart your computer.\n"
		"\n"
		"      "
	);

	while (*message != '\0') {
		buffer[len++] = *message;

		if (*message == '\n') {
			len += sprintf(buffer + len, "      ");
			++lines;
		}

		++message;
	}

	len += sprintf (buffer + len,
		       "\n\n      To help us improving our systems, please report this incident to us."
	);

	set_color(WHITE | RED << 4);
	clear_screen();

	gotoxy(0, (25 / 2) - (lines / 2) - 1);
	puts(buffer);
	printf("\n\n      ");
	halt();
}

/* easter egg! */
void winpanic(char *message)
{
	set_color(WHITE | BLUE << 4);
	clear_screen();

	gotoxy(37, 8);
	set_color(BLUE | LIGHT_GRAY << 4);
	printf(" Windows");
	set_color(WHITE | BLUE << 4);
	printf("\n\n      ");
	while (*message != '\0') {
		putchar(*message);

		if (*message == '\n') {
			printf("      ");
		}

		++message;
	}
	//printf("%s\n", message);

	printf("\n\n      *  Druecken Sie eine beliebige Taste, um die Anwendung abzubrechen.\n");
	printf("      *  Druecken Sie Strg+Alt+Entf, um den Computer neu zu\n");
	printf("      starten. nicht gespeicherte Daten gehen dabei verloren.\n");
	halt();
}
