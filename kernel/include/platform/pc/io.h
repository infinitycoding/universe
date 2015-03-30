#ifndef _IO_H_
#define _IO_H_

/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the Universe Kernel.

     The Universe Kernel is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.

     The Universe Kernel is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with the Universe Kernel. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <stdint.h>
#include <arch_context.h>

typedef uint16_t portid_t;

static inline uint8_t inb(uint16_t port)
{
    uint8_t result;
    asm volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t result;
    asm volatile("inw %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t result;
    asm volatile("inl %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

static inline void outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %1, %0" : : "Nd" (port), "a" (value));
}

static inline void outw(uint16_t port, uint16_t value)
{
    asm volatile("outw %1, %0" : : "Nd" (port), "a" (value));
}

static inline void outl(uint16_t port, uint32_t value)
{
    asm volatile("outl %1, %0" : : "Nd" (port), "a" (value));
}

int check_port(portid_t port);
int check_port_access(struct arch_thread_context *context, portid_t port);
int alloc_port(struct arch_thread_context *context, portid_t port);
void free_port(struct arch_thread_context *context, portid_t port);


#endif

