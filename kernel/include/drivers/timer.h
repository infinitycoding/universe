#ifndef _timer_h_
#define _timer_h_

/*
	Copyright 2012 universe coding group (UCG) all rights reserved
	This file is part of the Universe Kernel.

	Universe Kernel is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	Universe Kernel is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Universe Kernel.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
	@author Michael Sippel <micha.linuxfreak@gmail.com>
*/

#include <stdint.h>

typedef struct time {
	uint8_t second;
	uint8_t alarm_sec;
	uint8_t minute;
	uint8_t alarm_min;
	uint8_t hour;
	uint8_t alarm_hour;
	uint8_t week_day;
	uint8_t day_in_month;
	uint8_t month;
	uint8_t year;
	uint8_t century;
} time_t;

void INIT_PIT(int freq);
void INIT_RTC(void);

void rtc_irq_handler(void);
int update_time(void);
int change_time(time_t time);
time_t *get_time(void);
void set_pit_freq(int freq);
void print_time(time_t * time);

#endif

