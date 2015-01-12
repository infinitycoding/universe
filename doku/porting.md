Porting the kernel to any architecture
========

1. Provide a C compiler, assembler and bootloader(if necessary) for your platform
2. Adapt the makefiles and write a linkerscript (linkerscript_architecture.x)
3. Create a folder with the name of the target archicteture in "include/arch" and "arch/"
4. Implement the features given in hal.md. Put your HAL code in the folder you created
   in the last step
5. Modify init.c to remove/add initiaton calls for your modules



