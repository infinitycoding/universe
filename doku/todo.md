### TODO

##### General
* write testcases
* do testing
* write **doku doku doku**

##### Kernel
* add logging
* fix arch/x86/dump.c; more dumping
* safe FPU register
* make stack and heap unexecutable
* ASLR
* setup a rng deamon (kernel?)

##### Hypervisor
* page validation
* database for access rights
* use trigger module for pipes written to hypervisor

##### Capabilities
* capabilities für
	* syscall
	* excutable
	* packagemanager (handled in packege manger)
	* virtualisierung
* file access black and whitelisting und remaping
* jail
* capabilityedit

##### Mercury
* make it run on linux
* write **doku doku doku**
* define the missing functions for lua

###### minimal dependencie functions for liblua

	feof
	freopen
	ferror
	strtod
	strcoll
	strerror
	atan2
	ungetc
	clearerr
	setvbuf
	setjmp
	fscanf


