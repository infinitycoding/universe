Universe Syscalls
=============

#### Threads & Process

| ID   | Name			| ARG0 (Return) | ARG1 	| ARG2 	| ARG3 | ARG4 | Description |
| ---  | ---			| ---			| ---	| ---	| ---  | ---  |	---			|
|      | **exit**		|				|		|		|	   |	  |				|
| 0x00 | exit			|				|		|		|	   |	  |				|
| 0x01 | thread_exit	|				|		|		|	   |	  |				|
|      | **create**		|				|		|		|	   |	  |				|
| 0x02 | fork			|				|		|		|	   |	  |				|
| 0x03 | exec			|				|		|		|	   |	  |				|
| 0x04 | launch_threa	|				|		|		|	   |	  |				|
|      | **wait**		|				|		|		|	   |	  |				|

	<code> sleep</code>
	<code> waitpid</code>
	<code> set_trigger</code>

* FILES *

	// access
	<code> read</code>
	<code> write</code>

	// create&open
	<code> open</code>
	<code> pipe</code>
	<code> mknod</code>
	<code> mapfile</code>
	<code> close</code>

	// meta
	<code> chmod</code>
	<code> chown</code>
	<code> stat</code>

	// sockets
	<code> connect</code>
	<code> disconnect</code>
	<code> accept</code>
	<code> deny</code>

* CAPABILITIES *
	TODO
	set_capability
	remove_capability
