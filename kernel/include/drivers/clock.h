#ifndef _CLOCK_H_
#define _CLOCK_H_

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
	@author Michael Sippel <micha.linuxfreak@gmail.com>
*/

#include <event/time.h>
#include <drivers/cmos.h>


void INIT_CLOCK(int freq);
void INIT_PIT(int freq);
void INIT_RTC(void);
void set_pit_freq(int freq);
void sync_sys_clock();
void update_time(struct time *time);
void change_time(struct time *time);

#endif

