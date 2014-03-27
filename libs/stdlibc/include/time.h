#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

/*
     Copyright 2014 Infinitycoding all rights reserved
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
  @author Peter Hösch aka. BlitzBasic <phoesch@gmx.de>
*/



#include <stdlib.h>



//#define CLOCKS_PER_SEC 1000
#define NULL (void *) 0

// the following symbolic constants are not part of the standart implementation
// they are only for intern use and universe-only programs
#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define SECONDS_PER_HOUR (SECONDS_PER_MINUTE * MINUTES_PER_HOUR)
#define HOURS_PER_DAY 24
#define SECONDS_PER_DAY (SECONDS_PER_HOUR * HOURS_PER_DAY)
#define DAYS_PER_YEAR 365
#define DAYS_PER_LEAP-YEAR 366
#define SECONDS_PER_YEAR (SECONDS_PER_DAY * DAYS_PER_YEAR)
#define SECONDS_PER_LEAP-YEAR (SECONDS_PER_DAY * DAYS_PER_LEAP-YEAR)
#define DAYS_PER_WEEK 7



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



enum month	// not part of the standart implementation, only for intern use and universe-only programs
{
	january = 0,
	february = 1,
	march = 2,
	april = 3,
	may = 4,
	june = 5,
	july = 6,
	august = 7,
	september = 8,
	october = 9,
	november = 10,
	december = 11
};

enum weekday	// not part of the standart implementation, only for intern use and universe-only programs
{
	sunday = 0,
	monday = 1,
	tuesday = 2,
	wednesday = 3,
	thursday = 4,
	friday = 5,
	saturday = 6
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
