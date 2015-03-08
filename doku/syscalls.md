Universe Syscalls
=============

#### Threads & Process

| ID    | Name			| ARG0 (Return) | ARG1 	| ARG2 	| ARG3 	| ARG4  | Description 	|
| ---   | ---			| ---		| ---	| ---	| ---  	| ---   | ---		|
|       | **exit**		|		|	|	|       |	|               |
| 0x*0*00 | exit			|		|	|	|       |	|               |
| 0x*0*01 | thread_exit	        |		|	|	|       |       |               |
|       | **create**		|		|	|	|	|	|               |
| 0x*0*02 | fork			|		|	|	|	|	|               |
| 0x*0*03 | exec                   |		|	|	|	|	|               |
| 0x*0*04 | launch thread		|		|	|	|	|	|               |
|       | **wait**		|		|	|	|	|	|               |
| 0x*0*05 | sleep			|		|	|	|	|	|               |
| 0x*0*06 | waitpid		|		|	|	|	|	|               |
| 0x*0*07 | set_trigger		|		|	|	|	|	|               |

#### Shared Memory

| ID    | Name			| ARG0 (Return) | ARG1 	| ARG2 	| ARG3 	| ARG4  | Description 	|
| ---   | ---			| ---		| ---	| ---	| ---  	| ---   | ---		|
| 0x*1*01 | shm_get  |       |     |     |       |       |       |
| 0x*1*02 | shm_ctl  |       |     |     |       |       |       |
| 0x*1*03 | shm_attach |       |     |     |       |       |       |
| 0x*1*04 | shm_detach |       |     |     |       |       |       | 

#### Capabilities
TODO

| ID    | Name			| ARG0 (Return) | ARG1 	| ARG2 	| ARG3 	| ARG4  | Description 	|
| ---   | ---			| ---		| ---	| ---	| ---  	| ---   | ---		|
| 0x*2*01 | set_capability         |               |       |       |       |       |               |
| 0x*2*02 | remove_capability      |               |       |       |       |       |               |
