section .multiboot
align 4
dd 0x1BADB002
dd 0x0
dd -(0x1BADB002 + 0x0)
section .data
align 4096
boot_pd:
    dd 0x00000083
    times ((0xC0000000 >> 22) - 1) dd 0
    dd 0x00000083
    times (1024 - (0xC0000000 >> 22) - 1) dd 0
section .text
global start
start:
  mov ecx, boot_pd - 0xC0000000
  mov cr3, ecx
  mov ecx, cr4
  or ecx, (1 << 4)
  mov cr4, ecx
  mov ecx, cr0
  or ecx, (1 << 31)
  mov cr0, ecx
  lea ecx, [higherhalf]
  jmp ecx
higherhalf:
  mov esp, stack
  push 0
  push 0
  push eax
  push ebx
extern init
  call init
  jmp $
section .bss
align 4096
resb 4096
stack:
