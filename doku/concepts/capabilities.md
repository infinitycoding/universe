

In universe each executable programm or script must be part of a cell. A cell is a persitant capability context of a file or of a package of files.
It defines which files a programm can access and which system functions it can use by default.
Those rights and their heritability are assigned in black(explicitly prohibits access) lists, white(explicilty allows access) lists and Remap lists (remaps a specific path to another).
The rights of a process can be expanded by inheritage. When a process creates a new child process, it inherites all heritable rights to it by default. But the heritage can also explicitly definde
by the mother process to only inherit some and not all rights.
  

