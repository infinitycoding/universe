# UDRCP

UDRCP (Universe(al) driver communication protocoll) is the standart protocoll for driver 
communication in universe. UDRCP offers simple possibilites to communicate with drivers for standart devices
like file-systems, network-adapters, character-devices or PCI devices. It has also the capabillity to be extendet
with sub standarts for different devices. 

The protocoll is bidirectional and is in default binary (edianess hardware dependent) or UTF8.

### Connection establishing:

The the connection is usually established with process sockets in universe, but cann also be used via any
bi directional pipe system. 


##### socket folder:
- containing in and out file for main
- optional udrcp pipes/files

##### udrcp file:
- dma as readable memory block(s) as a file which is mapped to memory
- stream in and outputs (ports)


#### mapping on filesystem:
  open pipelines are mapped on the service hub
  ln from /dev to sockets in or outputs
  

### Levels:

|---------------------------------------|-------|
|                 Layer                 | Level |
|=======================================|=======|
|            Ports and Memory           |   0   |
|---------------------------------------|-------|
|         Bus Protocol / controler      |   1   |
|---------------------------------------|-------|
|           Midle level devices         |   2   |
|---------------------------------------|-------|
|  Abstract device / Highlevel Protocol |   3   |
|---------------------------------------|-------|

The protocol is splited up into 4 layers.
Layer 0 is about managing lowlevel perepheral access like DMA or
Port access.




#### Example:

PCI SSD

Port/memory access to controler         | Kernel Hypervisor
PCI controler driver                    | pci host
sending commands to controler           | ssd driver
decoding stored data from block device  | LVM
linking to filesystem                   | FS driver



### Packages:

|--------------|-------------|------------------|
|    offset    |    size     |     field        |
|--------------|-------------|------------------|
|     0x0      |   4 byte    |   Package ID     |
|--------------|-------------|------------------|
|     0x4      |   4 byte    |  Size of data    |
|--------------|-------------|------------------|
|     0x8      |   4 byte    | Type/Errorcode   |
|--------------|-------------|------------------|
|     0xc      | size byte   |   Data area      |
|--------------|-------------|------------------|



### Universa Types:


|---------|---------------------|
|  value  |         typ         |
|---------|---------------------|
|   0x0   |   connection reset  |
|---------|---------------------|
|   0x1   |   connection break  |
|---------|---------------------|
|   0x2   |    share memory     |
|---------|---------------------|
|   0x3   |    pass memory      |
|---------|---------------------|
|   0x4   |    list accessors   |
|---------|---------------------|

connection reset:

connection break:

share memory:

passed memory:


### Level 0 Types:

|---------|---------------------|
|  value  |         typ         |
|---------|---------------------|
|   0x5   |   query DMA memory  |
|---------|---------------------|
|   0x6   |   free DMA memory   |
|---------|---------------------|
|   0x7   |   query irq singal  |
|---------|---------------------|
|   0x8   |   free irq handle   |
|---------|---------------------|
|   0x9   |  query port access  |
|---------|---------------------|
|   0xA   |   free port access  |
|---------|---------------------|
|   0xB   |   read remote port  |
|---------|---------------------|
|   0xC   | write to remote port|
|---------|---------------------|



**hosted Ports**:
ports which aren't accessible directly. Acces is performed via UDRCP Pipline

**direct ports**:
ports which can directly be accessed via CPU instruction

**direct memory**:

**hosted memory**:



### Level 1:

|---------|----------------------|
|  value  |         typ          |
|---------|----------------------|
|   0x5	  | get attatched devices|
|---------|----------------------|
|   0x6   | alloc device         |
|---------|----------------------|
|   0x7   |  free device         |
|---------|----------------------|
|   0x8   |  alloc pci memory    |
|---------|----------------------|
|   0x9   | free pci memory      |
|---------|----------------------|

|---------|---------------------|

|---------|---------------------|

PCI:

ISA:

USB:

SATA/SCSI/SAS:



### Level 2 Types:


Block Device:

|---------|-----------------------|
|  value  |         typ           |
|---------|-----------------------|
|   0x5	  | get device Information|
|---------|-----------------------|
|   0x6   |read block		  |
|---------|-----------------------|
|  0x7	  | write block	          |
|---------|-----------------------|

Stream Device:

LVM:


### Level 3 Types:


File system:

|---------|---------------------|
|  value  |         typ         |
|---------|---------------------|
|         |    create     |
|---------|---------------------|
|         |    delete     |
|---------|---------------------|
|         |    rename     |
|---------|---------------------|
|         |    mv     |
|---------|---------------------|
|         |    cp     |
|---------|---------------------|




rendering device / Coprocessor:

asynchronus callstack:

synchronus callstack:

compile kernel:

upload kernel:






network:

|---------|---------------------|
|  value  |         typ         |
|---------|---------------------|
|         |    send package     |
|---------|---------------------|
|         |  dispatch package   |
|---------|---------------------|


### establishing a connection:


### shuting down pipline:


Abbildung von root piplines in /dev



