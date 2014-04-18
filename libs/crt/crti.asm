
SECTION .init
global _init
align 16

_init:
push ebp
mov esp,ebp
sub esp,8


SECTION .fini
global _fini
align 16
_fini:
push ebp
mov ebp,esp

