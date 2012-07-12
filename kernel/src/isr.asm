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

Global int_0
Global int_1
Global int_2
Global int_3
Global int_4
Global int_5
Global int_6
Global int_7
Global int_8
Global int_9
Global int_10
Global int_11
Global int_12
Global int_13
Global int_14
Global int_15
Global int_16
Global int_17
Global int_18
Global int_19
Global int_20
Global int_21
Global int_22
Global int_23
Global int_24
Global int_25
Global int_26
Global int_27
Global int_28
Global int_29
Global int_30
Global int_31

;Divide by Zero
int_0:
push dword 0
push dword 0
jmp exception_save

;Debug Exception
int_1:
push dword 0
push dword 1
jmp exception_save

;Non Maskable Interrupt
int_2:
push dword 0
push dword 2
jmp exception_save

;Breakpoint Exception
int_3:
push dword 0
push dword 3
jmp exception_save

;Overflow Exception
int_4:
push dword 0
push dword 4
jmp exception_save

;Bound Range Exception
int_5:
push dword 0
push dword 5
jmp exception_save

;Invalid Opcode
int_6:
push dword 0
push dword 6
jmp exception_save

;Device Not Available
int_7:
push dword 0
push dword 7
jmp exception_save

;Double Fault
int_8:
push dword 8
jmp exception_save

;Coprocessor Segment Overrun Exception
int_9:
push dword 0
push dword 9
jmp exception_save

;Invalid TSS
int_10:
push dword 10
jmp exception_save

;Segment not Present
int_11:
push dword 11
jmp exception_save

;Stack Fault
int_12:
push dword 12
jmp exception_save

;General Protection Fault
int_13:
push dword 13
jmp exception_save

;Page Fault
int_14:
push dword 14
jmp exception_save

;Reserved
int_15:
push dword 0
push dword 15
jmp exception_save

;x87 Floating Point Exception
int_16:
push dword 0
push dword 16
jmp exception_save

;Alignment Check
int_17:
push dword 17
jmp exception_save

;Machine Check
int_18:
push dword 0
push dword 18
jmp exception_save

;SIMD Floating Point
int_19:
push dword 0
push dword 19
jmp exception_save

;Reserved
int_20:
push dword 0
push dword 20
jmp exception_save

;Reserved
int_21:
push dword 0
push dword 21
jmp exception_save

;Reserved
int_22:
push dword 0
push dword 22
jmp exception_save

;Reserved
int_23:
push dword 0
push dword 23
jmp exception_save

;Reserved
int_24:
push dword 0
push dword 24
jmp exception_save

;Reserved
int_25:
push dword 0
push dword 25
jmp exception_save

;Reserved
int_26:
push dword 0
push dword 26
jmp exception_save

;Reserved
int_27:
push dword 0
push dword 27
jmp exception_save

;Reserved
int_28:
push dword 0
push dword 28
jmp exception_save

;Reserved
int_29:
push dword 0
push dword 29
jmp exception_save

;Reserved
int_30:
push dword 0
push dword 30
jmp exception_save

;Reserved
int_31:
push dword 0
push dword 31
jmp exception_save




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

Global irq_0
Global irq_1
Global irq_2
Global irq_3
Global irq_4
Global irq_5
Global irq_6
Global irq_7
Global irq_8
Global irq_9
Global irq_10
Global irq_11
Global irq_12
Global irq_13
Global irq_14
Global irq_15



irq_0:
push dword 0
push dword 0
jmp int_save

irq_1:
push dword 0
push dword 1
jmp int_save

irq_2:
push dword 0
push dword 2
jmp int_save

irq_3:
push dword 0
push dword 3
jmp int_save

irq_4:
push dword 0
push dword 4
jmp int_save

irq_5:
push dword 0
push dword 5
jmp int_save

irq_6:
push dword 0
push dword 6
jmp int_save

irq_7:
push dword 0
push dword 7
jmp int_save

irq_8:
push dword 0
push dword 8
jmp int_save

irq_9:
push dword 0
push dword 9
jmp int_save

irq_10:
push dword 0
push dword 10
jmp int_save

irq_11:
push dword 0
push dword 11
jmp int_save

irq_12:
push dword 0
push dword 12
jmp int_save

irq_13:
push dword 0
push dword 13
jmp int_save

irq_14:
push dword 0
push dword 14
jmp int_save

irq_15:
push dword 0
push dword 15
jmp int_save

extern irq_handler
irq_handler:
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
