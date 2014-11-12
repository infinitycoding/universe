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
 *  @file /arch/x86/io.c
 *  @brief Functions for allocation port access for usermode processes.
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */ 

#include <io.h>
#include <tss.h>

uint32_t global_iobmp[IO_BITMAP_LENGTH / 32] = {0};
// 1 = taken, 0 = free (not like x86 io bitmap)

/**
 * @brief Checks if the port is already taken 
 * @param port	The port id
 * @return 1 if the port is available; 0 if the port is already taken
 */
int check_port(portid_t port)
{
	return !(global_iobmp[port/32] & (1<< (port & 31)));
}

/**
 * @brief Checks if the acces to a port is granted in context.  
 * @param context 	Thread context which should be checked
 * @param port 		The port id
 * @return 1 if access is granted; 0 if access is denied
 */
int check_port_access(struct arch_thread_context *context, portid_t port)
{
	return !(context->ports.iobmp[port/32] & (1<< (port & 31)));
}

/**
 * @brief Allocates acces to port for context if the port is not already allocated
 * @param context 	Thread context which should get access
 * @param port 		The port id
 * @return 1 if access is granted; 0 if access is denied/already taken
 */
int alloc_port(struct arch_thread_context *context, portid_t port)
{
	if(check_port(port))
	{
		global_iobmp[port/32] |=  1<< (port & 31);
		context->ports.iobmp[port/32] &= ~(1<< (port & 31));
	}
	return check_port_access(context, port); // if the port has already been allocated, it will return 1 again
}

/**
 * @brief Removes the access rights for a port from context 
 * @param context 	Thread context which should loose access
 * @param port 		The port id
 */
void free_port(struct arch_thread_context *context, portid_t port)
{
	if(check_port_access(context,port))
	{
		context->ports.iobmp[port/32] |= 1<<(port & 31);
		global_iobmp[port/32] &= ~(1<< (port & 31));
	}
}
