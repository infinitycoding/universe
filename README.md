universe
========

Universe is a small unix-compatible operating system kernel.
It can nativly run linux programs und has a very generic driver pipeline system.

Licenses: 
  Kernel: GNU GPL v.3
  Libraries: GNU LGPL

If you find any kind of bug please report it to info@infinitycoding.de

Dependencies:
========
* gcc   	            (only for compiling)
* nasm  	            (only for compiling)
* qemu		            (only for testing)
* cdrkit/genisoimage	(only for compiling)
* make					(only for compiling)


How to compile:
========

````
	make dependencies
	make all
````

run in qemu vm:
========

````
	make qemu
````

