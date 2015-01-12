GDT module and concepts
=======

GDT Concept
-----------
In x86 compatible CPUs exists a data structure called Global Descriptor Table (GDT).
The GDT consists of datatructures with the lengh of 8byte so called descriptors.
The first descriptor of the GDT is the NULL-Descriptor(all bytes = 0). The other descriptors
descibe memorysegments for setting access rights and gates for accessing syscalls of the kernel from 
userspace. After setting up the entries the basic information about the GDT (Base address and size)
have to be loaded to the GDT-Register (GDTR). It's also necessary to perform a jump into the new segments to activate them.

###Code and Data Segments
The code and data segments are used to set a address space ontop of the physical memory.
The data segment decribes the address space in which data is addressed and the code segment
for addressing executable code.The address space begins with the address 0 and is mapped to the given physical
base address and ends add the base+size*granularity(4K or 4M). The segments also seperate previleg levels.
A programm that is running in a usermode codesegment has less rights then in a kernelmode segment.
For example it can't change GDT,IDT or CPU mode without creating a execption. 

###Task State Segment
The tas state segment was used for hardware multitasking. But for compatability and flexibility reasons
it is used just a table to store the the kernelstack address and the IO acces bitmap. 

###Call Gates
Call gates are used to call a Kernel API function from userspace. 


GDT Module
-----------
The GDT module sets up two code and data segments and one task state segment.
One code and one datasegment is for userspace the others for kernelspace.
In the current Implementation all usermode threads use the same segments.
Only the thread state (IO BMP, Memory context and CPU state) will be swapped.
The whole memorymanagement is done 


 | - | Current GDT Layout: | 
 | - | ------------------- | 
 | 0 |      Null Desc.     | 
 | 1 |  Kernel Code Desc.  | 
 | 2 |  Kernel Data Desc.  | 
 | 3 |   User Code Desc.   | 
 | 4 |   User Data Desc.   | 
 | 5 |      TSS Desc.      | 



###Todo:
- Use more segments to mark only the executable code as executable to prevent bufferoverflow exploits.
- Implement Kernel ASLR



