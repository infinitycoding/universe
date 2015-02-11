Security
========

The most common available operating systems kernel are huge and have a monolithic kernel desing.
The bad thing about that is you have to trust many million lines of code. All of them must be
written very carfully. The probability that there is an error which can be exploited is huge.
Of course universe has more than just a few lines but documentation and distribution inside 
the operatng system matters. The kernel of an operating sytem is the instance which has the most 
rights on your computer. If a programm can exploit this part of the operating system the arbitrary code
can control the whole machine an could even embedd itself into your hardwares firmware.
That's why our kernel follows the microkernel concept. A micro kernel is as small as possible
to reduce the attackable area and puts the rest of it's functionality onto user level which is better to control.
What the most unix like systems do is a pretty good example for security. But for us it's not enough.
It does not just matter which user is running is a programm, the programm itself is important aswell.
A browser shouldnt ever have the rights to access your private files.
To do so there is a container system which seperates programms including it's dependencies from each other,
a capability right system which can restrict the rights of each container with a really fine granularity
ontop of the reliable unix filesystem right management makes it really dificult for attacks. 
Thoose functionalities can be used to easyly seperate software from each other and provide a better system security.
But that's not everything. We belive that we can only completly trust a system if we know how everything 
inside works.
To make universe a trustfull platform we document all functionality and structures inside our software,
the platform on which it is running, and how to develop software/extensions for it. Our goal is that it only 
should take a few days to understand the code of the kernel, if you are a little bit into operating system design.
The you should be able to port the kernel to your favorite platform within a few hours.


Communication
========
The internal socketsystem provides a way of letting server communicate with clients on a scaleable way.
(tunneling via TCP/IP or any other protocoll). Conntecting to the internet is quiet Easy: connect("/dev/net/192.168.178.0")
There is also a new protocoll calles UDRCP which allows to connect varios drivers to each other no matter where the devices are
(maybe on another computer) or how weird your platform is. The socket system is also used to provides high performance interprocess 
communication like DMA or (a)synchronus batched calls which also could be used by udrcp.


Usability
========
Rolling release and a usabel packet manager
user inteface shows devices before mounting (preventing bad usb or TB)
man entries for paths and files