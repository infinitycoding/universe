extern main

_start:
	call main
	mov ebx, eax
	mov eax, 1
	int 0x80
	jmp $

