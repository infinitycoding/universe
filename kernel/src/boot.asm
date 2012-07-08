;	Copyright 2012 the developers of the universe
;	This file is part of Universe Kernel.
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
;    Diese Datei ist Teil vom Universe Kernel.
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

[BITS 32]
section .text
FLAGS    equ 0
MAGIC    equ 0x1BADB002       ; Magicnumber - Erkennungsmerkmal für GRUB
CHECKSUM equ -(MAGIC + FLAGS) ; Checksum
zeilen equ 10

 
align 4
MultiBootHeader:
  dd MAGIC       ; Magic number
  dd FLAGS       ; Flags
  dd CHECKSUM    ; Checksu



Global start
start:
cli
mov [MBS],ebx
mov esp,stack
cmp eax,0x2BADB002
jne halt

extern init
call init

halt:
hlt
jmp $

section .data
Global MBS
MBS dd 0

section .bss
resb 4096
stack:


