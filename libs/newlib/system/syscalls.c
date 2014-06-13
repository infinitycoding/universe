/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the universe newlib patch.
 
     The universe newlib patch is free software: you can redistribute it and/or modify
     it under the terms of the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.
 
     The universe newlib patch is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser General Public License for more details.
 
     You should have received a copy of the GNU Lesser General Public License
     along with the universe newlib patch.  If not, see <http://www.gnu.org/licenses/>.
 */


/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>
#include <universe.h>

#undef errno
extern int errno;
char **environ; /* pointer to array of char * strings that define the current environment variables */

void _exit()
{
    asm volatile("int $128" : : "a"(SYS_EXIT), "b"(0));
}

int close(int file)
{
    return linux_syscall(SYS_CLOSE, file, 0, 0, 0, 0);
}


int execve(char *name, char **argv, char **env)
{
    return linux_syscall(SYS_EXECVE, (uint32_t)name, (uint32_t)argv, (uint32_t)env, 0, 0);
}

int fork()
{
    int pid;
    asm volatile("int $128;": "=a"(pid) : "a" (SYS_FORK));
    return pid;
}

int fstat(int file, struct stat *st)
{
     return linux_syscall(SYS_FSTAT, file, (uint32_t) st, 0, 0, 0);
}

int getpid()
{
    return linux_syscall(SYS_GETPID, 0, 0, 0, 0, 0);
}

int isatty(int file)
{
    return 0;
}

int kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}

int link(char *old, char *new)
{
    return linux_syscall(SYS_LINK, (uint32_t)old, (uint32_t)new, 0, 0, 0);
}

int lseek(int file, int ptr, int dir)
{
    return linux_syscall(SYS_LSEEK, (uint32_t)file, (uint32_t)ptr, (uint32_t)dir, 0, 0);
}

int open(const char *name, int flags, , int mode)
{
     return linux_syscall(SYS_OPEN, (uint32_t)name, flags, mode, 0, 0);
}

int read(int file, char *ptr, int len)
{
     return linux_syscall(SYS_READ, file, (uint32_t)ptr, len, 0, 0);
}

caddr_t sbrk(int incr)
{
    void *heap_end = (void *) linux_syscall(SYS_BRK, 0, 0, 0, 0, 0);
    caddr_t r = (caddr_t) linux_syscall(SYS_BRK,(uint32_t) heap_end+incr, 0, 0, 0, 0);
    if(!r)
    {
        write (2, "Heap/stack collision or freeing of unallocated blocks\n", 54);
        abort ();
    }
    return t;
}

int stat(const char *file, struct stat *st)
{
    return linux_syscall(SYS_STAT, (uint32_t)file, (uint32_t)st, 0, 0, 0);
}

clock_t times(struct tms *buf)
{
    return -1;
}

int unlink(char *name)
{
    return linux_syscall(SYS_UNLINK, (uint32_t)name, 0, 0, 0, 0);
}

int wait(int *status)
{
    return linux_syscall(SYS_WAITPID, *status, 0, 0, 0, 0);
}

int write(int file, char *ptr, int len)
{
    return linux_syscall(SYS_WRITE, file, (uint32_t)ptr, len, 0, 0);
}

int gettimeofday(struct timeval *p, struct timezone *z)
{
    if(p)
    {
        p->tv_sec = linux_syscall(SYS_TIME, 0, 0, 0, 0, 0);
        p->tv_usec = 0; // kernel can't log usecs at the time of implementation
    }

    if(z)
    {
        z->tz_minuteswest = 23*60; // timeshift westwards from GMT -> timezone of middle europe
        z->tz_dsttime = 4; //
    }
    return 0;
}