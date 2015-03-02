Universe Syscalls
=============

#### Threads & Process

| ID   | Name			| ARG0 (Return) | ARG1 	| ARG2 	| ARG3 	| ARG4  | Description 	|
| ---  | ---			| ---		| ---	| ---	| ---  	| ---   | ---		|
|      | **exit**		|		|	|	|       |	|               |
| 0x00 | exit			|		|	|	|       |	|               |
| 0x01 | thread_exit	        |		|	|	|       |       |               |
|      | **create**		|		|	|	|	|	|               |
| 0x02 | fork			|		|	|	|	|	|               |
| 0x03 | exec                   |		|	|	|	|	|               |
| 0x04 | launch thread		|		|	|	|	|	|               |
|      | **wait**		|		|	|	|	|	|               |
| 0x05 | sleep			|		|	|	|	|	|               |
| 0x06 | waitpid		|		|	|	|	|	|               |
| 0x07 | set_trigger		|		|	|	|	|	|               |

#### Files

| ID   | Name			| ARG0 (Return) | ARG1 	| ARG2 	| ARG3 	| ARG4  | Description 	|
| ---  | ---			| ---		| ---	| ---	| ---  	| ---   | ---		|
|      | **access**		|		|	|	|	|	|	        |
| 0x08 | read                   |		|	|	|	|	|	        |
| 0x09 | write			|		|	|	|	|	|       	|
|      | **create & open**	|		|	|	|	|	|	        |
| 0x0A | open			|		|	|	|	|	|       	|
| 0x0B | pipe			|		|	|	|	|	|       	|
| 0x0C | mknod			|		|	|	|	|	|       	|
| 0x0D | mapfile		|		|	|	|	|	|       	|
| 0x0E | close			|		|	|	|	|	|       	|
|      | **meta**		|		|	|	|	|	|		|
| 0x0F | chmod			|		|	|	|	|	|       	|
| 0x10 | chown			|		|	|	|	|	|       	|
| 0x11 | stat			|		|	|	|	|	|       	|
|      | **sockets**            |               |       |       |       |       |               |
| 0x12 | connect                |               |       |       |       |       |               |
| 0x13 | disconnect             |               |       |       |       |       |               |
| 0x14 | accept                 |               |       |       |       |       |               |
| 0x15 | deny                   |               |       |       |       |       |               |

#### Capabilities
TODO

| ID   | Name			| ARG0 (Return) | ARG1 	| ARG2 	| ARG3 	| ARG4  | Description 	|
| ---  | ---			| ---		| ---	| ---	| ---  	| ---   | ---		|
| 0x16 | set_capability         |               |       |       |       |       |               |
| 0x17 | remove_capability      |               |       |       |       |       |               |
