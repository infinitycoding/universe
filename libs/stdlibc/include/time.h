#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

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

	You should have received a copy of thCLOCKS_PER_SECe GNU General Public License
	along with Universe Kernel.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
  @author Peter Hösch aka. BlitzBasic <phoesch@gmx.de>
*/



//#define CLOCKS_PER_SEC 1000



typedef unsigned int time_t;
typedef unsigned int clock_t;
typedef unsigned int size_t;



struct tm
{
	int tm_sec;		// seconds
	int tm_min;		// minutes
	int tm_hour;		// hours
	int tm_mday;		// day of the month (0 - 31)
	int tm_mon;		// month (0 - 11)
	int tm_year;		// years since 1900
	int tm_wday;		// days since sunday (0 - 6)
	int tm_yday;		// day since the first january (0 - 365)
	int tm_isdst;		// summer or winter time
};



time_t time(time_t *timer);

struct tm *localtime(const time_t *timer);
struct tm *gmtime(const time_t *timer);

time_t mktime(struct tm *timer);
char *asctime(struct tm *timer);

char *ctime(const time_t *timer);

double difftime(time_t time1, time_t time0);

clock_t clock(void);

size_t strftime(char *buffer, int maxchars, const char *format, struct tm *timer);



#endif
