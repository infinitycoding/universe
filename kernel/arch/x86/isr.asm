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

%macro ISR 1
	Global isr_%1
	isr_%1:
	push dword 0
	push dword %1
	jmp isr_save
%endmacro

%macro ISR_ERROR_CODE 1
	Global isr_%1
	isr_%1:
	push dword %1
	jmp isr_save
%endmacro

;Divide by Zero
	ISR 0
;Debug Exception
	ISR 1
;Non Maskable Interrupt
	ISR 2
;Breakpoint Exception
	ISR 3
;Overflow Exception
	ISR 4
;Bound Range Exception
	ISR 5
;Invalid Opcode
	ISR 6
;Device Not Available
	ISR 7
;Double Fault
	ISR_ERROR_CODE 8
;Coprocessor Segment Overrun Exception
	ISR 9
;Invalid TSS
	ISR_ERROR_CODE 10
;Segment not Present
	ISR_ERROR_CODE 11
;Stack Fault
	ISR_ERROR_CODE 12
;General Protection Fault
	ISR_ERROR_CODE 13
;Page Fault
	ISR_ERROR_CODE 14
;Reserved
	ISR 15
;x87 Floating Point Exception
	ISR 16
;Alignment Check
    ISR_ERROR_CODE 17
;Machine Check
	ISR 18
;SIMD Floating Point
	ISR 19
;Reserved
	ISR 20
;Reserved
	ISR 21
;Reserved
	ISR 22
;Reserved
	ISR 23
;Reserved
	ISR 24
;Reserved
	ISR 25
;Reserved
	ISR 26
;Reserved
	ISR 27
;Reserved
	ISR 28
;Reserved
	ISR 29
;Reserved
	ISR 30
;Reserved
	ISR 31


;Default
    Global isr_default
	isr_default:
	push dword 0xFFFFFFFF
	push dword 0xFFFFFFFF
	jmp isr_save


;Timer Interrupt / Task Scheduler
    ISR 32
    ISR 33
    ISR 34
    ISR 35
    ISR 36
    ISR 37
    ISR 38
    ISR 39
    ISR 40
    ISR 41
    ISR 42
    ISR 43
    ISR 44
    ISR 45
    ISR 46
    ISR 47
;Universe Syscall Handler
    ISR 112

;Linux Syscall Handler
    ISR 128


Global isr_pci
isr_pci:
push dword 0
push dword 50
jmp isr_save


extern irq_handler
isr_save:
push eax
push ecx
push edx
push ebx
push ebp
push esi
push edi
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
pop edi
pop esi
pop ebp
pop ebx
pop edx
pop ecx
pop eax
add esp, 8
iret
