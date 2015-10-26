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
 *  @file drivers/pit/pit.c
 *  @brief Kernel driver for the programmable interrupt timer (PIT)
 *  @author Michael Sippel <micha@infinitycoding.de>
 */
#include <drivers/pit.h>
#include <io.h>

/**
 * @brief Initalize the Programmable Intervall Timer.
 * @param frequency
 * @return void
 */
void INIT_PIT(int freq)
{
    outb(0x43, 0x34);
    set_pit_freq(freq);
}

/**
 * @brief set PIT Fequency.
 * @param freqency
 * @return void
 */
void set_pit_freq(int freq)
{
    int counter = 1193182 / freq;
    outb(0x40,counter & 0xFF);
    outb(0x40,counter >> 8);
}

