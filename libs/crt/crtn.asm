section .init
mov esp, ebp
pop ebp
ret

section .fini
pop ecx
pop ebx
mov esp, ebp
pop ebp
ret