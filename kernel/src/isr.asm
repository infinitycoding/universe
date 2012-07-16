;	Copyright 2012 universe coding group (UCG) all rights reserved
;	This file is part of the Universe Kernel.
;
;    Universe Kernel is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    any later version.
;
;    Universe Kernel is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with Universe Kernel.  If not, see <http://www.gnu.org/licenses/>.
;
;	
;	
;    Diese Datei ist ein Teil vom Universe Kernel.
;
;    Das Universe Kernel ist Freie Software: Sie können es unter den Bedingungen
;    der GNU General Public License, wie von der Free Software Foundation,
;    Version 3 der Lizenz oder jeder späteren
;    veröffentlichten Version, weiterverbreiten und/oder modifizieren.
;
;    Das Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
;    Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
;    OHNE JEDE GEWÄHELEISTUNG, bereitgestellt; sogar ohne die implizite
;    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
;    Siehe die GNU General Public License für weitere Details.
;
;    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
;    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.

%macro EXCEPTION 1
	Global int_%1
	int_%1:
	push dword 0
	push dword %1
	jmp exception_save
%endmacro

%macro EXCEPTION_ERROR_CODE 1
	Global int_%1
	int_%1:
	push dword %1
	jmp exception_save
%endmacro

;Divide by Zero
	EXCEPTION 0
;Debug Exception
	EXCEPTION 1
;Non Maskable Interrupt
	EXCEPTION 2
;Breakpoint Exception
	EXCEPTION 3
;Overflow Exception
	EXCEPTION 4
;Bound Range Exception
	EXCEPTION 5
;Invalid Opcode
	EXCEPTION 6
;Device Not Available
	EXCEPTION 7
;Double Fault
	EXCEPTION_ERROR_CODE 8
;Coprocessor Segment Overrun Exception
	EXCEPTION 9
;Invalid TSS
	EXCEPTION_ERROR_CODE 10
;Segment not Present
	EXCEPTION_ERROR_CODE 11
;Stack Fault
	EXCEPTION_ERROR_CODE 12
;General Protection Fault
	EXCEPTION_ERROR_CODE 13
;Page Fault
	EXCEPTION_ERROR_CODE 14
;Reserved
	EXCEPTION 15
;x87 Floating Point Exception
	EXCEPTION 16
;Alignment Check
	EXCEPTION_ERROR_CODE 17
;Machine Check
	EXCEPTION 18
;SIMD Floating Point
	EXCEPTION 19
;Reserved
	EXCEPTION 20
;Reserved
	EXCEPTION 21
;Reserved
	EXCEPTION 22
;Reserved
	EXCEPTION 23
;Reserved
	EXCEPTION 24
;Reserved
	EXCEPTION 25
;Reserved
	EXCEPTION 26
;Reserved
	EXCEPTION 27
;Reserved
	EXCEPTION 28
;Reserved
	EXCEPTION 29
;Reserved
	EXCEPTION 30
;Reserved
	EXCEPTION 31

extern exception_handler
exception_save:
pusha
push ds
push es
push fs
push gs
mov dx,0x10
mov ds, dx
mov es, dx
mov fs, dx
mov gs, dx
push esp
call exception_handler
mov esp,eax
pop gs
pop fs
pop es
pop ds
popa
add esp, 8
iret

%macro IRQ 1
	Global irq_%1
	irq_%1:
	push dword 0
	push dword %1
	jmp int_save
%endmacro

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

extern irq_handler
int_save:
pusha
push ds
push es
push fs
push gs
mov dx,0x10
mov ds, dx
mov es, dx
mov fs, dx
mov gs, dx
push esp
call irq_handler
mov esp,eax
pop gs
pop fs
pop es
pop ds
popa
add esp, 8
iret
