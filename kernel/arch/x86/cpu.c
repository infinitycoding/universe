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
 *  @file /arch/x86/cpu.c
 *  @brief Architecture dependent functions.
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */


/**
 * @brief Stops the execution of the kernel and disables interrupts.
 */
void halt(void)
{
    while (1)
    {
        asm volatile("cli; hlt");
    }
}

/**
 * @brief Disables interrupts. 
 */
void disable_irqs(void)
{
    asm volatile("cli");
}


/**
 * @brief Enables interrupts.
 */
void enable_irqs(void)
{
    asm volatile("sti");
}

