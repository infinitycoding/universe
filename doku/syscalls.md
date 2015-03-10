Universe Syscalls
=============

#### Threads & Process

| ID    | Name                      | ARG0 (Return) | ARG1  | ARG2  | ARG3  | ARG4  | Description   |
| ---   | ---                       | ---           | ---   | ---	| ---  	| ---   | ---           |
|       | **exit**                  |               |       |       |       |       |               |
| 0x000 | exit                      |               |       |       |       |       |               |
| 0x001 | thread_exit               |               |       |       |       |       |               |
|       | **create**                |               |       |       |       |       |               |
| 0x002 | fork                      |               |       |       |       |       |               |
| 0x003 | exec                      |               |       |       |	    |       |               |
| 0x004 | launch thread             |               |       |       |	    |       |               |
|       | **wait**                  |               |       |       |	    |       |               |
| 0x005 | sleep                     |               |       |       |	    |       |               |
| 0x006 | waitpid                   |               |       |       |	    |       |               |
| 0x007 | set_trigger               |               |       |       |	    |       |               |

#### Shared Memory

| ID    | Name                      | ARG0 (Return) | ARG1  | ARG2  | ARG3  | ARG4  | Description   |
| ---   | ---                       | ---           | ---   | ---   | ---   | ---   | ---           |
| 0x101 | shm_get                   |               |       |       |       |       |               |
| 0x102 | shm_ctl                   |               |       |       |       |       |               |
| 0x103 | shm_attach                |               |       |       |       |       |               |
| 0x104 | shm_detach                |               |       |       |       |       |               | 


#### Signals

| ID    | Name                      | ARG0 (Return) | ARG1  | ARG2  | ARG3  | ARG4  | Description   |
| ---   | ---                       | ---           | ---   | ---   | ---   | ---   | ---           |
| 0x200 | set_signal                |               |       |       |       |       |               |
| 0x201 | fetch_signal              |               |       |       |       |       |               |


#### Capabilities

| ID    | Name                      | ARG0 (Return) | ARG1  | ARG2  | ARG3  | ARG4  | Description   |
| ---   | ---                       | ---           | ---   | ---   | ---   | ---   | ---           |
| 0x300 | get_capabilities          |               |       |       |       |       |               |
| 0x301 | set_capability            |               |       |       |       |       |               |
| 0x302 | remove_capability         |               |       |       |       |       |               |
