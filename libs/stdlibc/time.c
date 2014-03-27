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



#include <time.h>

#include <universe.h>
#include <string.h>
#include <stdlib.h>
#include <atoi.h>



char static_string[STATIC_TIME_STRING_LENGTH];
struct tm static_tm;



time_t time(time_t *timer)
{
	time_t t = (time_t)linux_syscall(SYS_TIME, 0, 0, 0, 0, 0);

	if(timer != NULL)
		*timer = t;

	return t;
}


struct tm *localtime(const time_t *timer)
{
	// TODO

	return NULL;
}


struct tm *gmtime(const time_t *timer)
{
	struct tm *decoded_time = (struct tm *) malloc(sizeof(struct tm));
	
	int days = *timer / SECONDS_PER_DAY;
	int remaining_seconds = *timer % SECONDS_PER_DAY;

	// TODO
	
	decoded_time->tm_sec = (remaining_seconds % SECONDS_PER_MINUTE);
	decoded_time->tm_min = ((remaining_seconds % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE);
	decoded_time->tm_hour = (remaining_seconds / SECONDS_PER_HOUR);
	decoded_time->tm_wday = (thursday + (days % DAYS_PER_WEEK));

	return decoded_time;
}


time_t mktime(struct tm *timer)
{
	// TODO

	return 0;
}


char *asctime(struct tm *timer)
{
	memset(static_string, ' ', sizeof(char) * STATIC_TIME_STRING_LENGTH);

	strncpy(static_string[0], weekday_name_short[timer->tm_weday], 4);
	strncpy(static_string[5], month_name_short[timer->tm_mon], 4);
	itoa(timer->tm_mday, static_string[10], 10);
	itoa(timer->tm_hour, static_string[13], 10);
	static_string[15] = ':';
	itoa(timer->tm_min, static_string[16], 10);
	static_string[18] = ':';
	itoa(timer->tm_sec, static_string[19], 10);
	itoa(timer->tm_year + 1900, static_string[23], 10);
	static_string[27] = '\n';
	static_string[28] = '\0';

	return static_string;
}


char *ctime(const time_t *timer)
{
	return asctime(localtime(timer));
}


double difftime(time_t time1, time_t time0)
{
	return ((double)(time1 - time0));
}


clock_t clock()
{
	// TODO

	return -1;
}


size_t strftime(char *buffer, int maxchars, const char *format, struct tm *timer)
{
	// TODO

	return 0;
}


int validtm(const struct tm *timer)
{
	if(timer == NULL)
		return INVALID_TIME;

	if(timer->tm_sec < 0 || timer->tm_sec > 59 || timer->tm_min < 0 || timer->tm_min > 59 || timer->tm_hours < 0 || timer->tm_hours > 23 || timer->tm_mday < 1 || timer->tm_mday > 31 || timer->tm_year < 70 || timer->tm_wday < 0 || timer->tm_wday > 6 || timer->tm_yday < 0 || timer->tm_yday > 365 || (timer->tm_yday > 364 && (timer->tm_yday % 4 != 0 || (timer->tm_yday % 100 == 0 && (timer->tm_yday + 1900) % 400 != 0))))
		return INVALID_TIME;

	return VALID_TIME;
}


int validtimestamp(const time_t *timer)
{
	if(timer == NULL)
		return INVALID_TIME;

	if(timer < 0)
		return INVALID_TIME;

	return VALID_TIME;
}



