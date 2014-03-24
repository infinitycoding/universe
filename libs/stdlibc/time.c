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
  @author Peter Hösch aka. BlitzBasic <phoesch@gmx.de>
*/



#include <time.h>
#include <universe.h>



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
	// TODO

	return NULL;
}


time_t mktime(struct tm *timer)
{
	// TODO

	return 0;
}


char *asctime(struct tm *timer)
{
	// TODO

	return NULL;
}


char *ctime(const time_t *timer)
{
	// TODO

	return NULL;
}


double difftime(time_t time1, time_t time0)
{
	return ((double)(time1 - time0));
}


clock_t clock()
{
	// TODO

	return 0;
}


size_t strftime(char *buffer, int maxchars, const char *format, struct tm *timer)
{
	// TODO

	return 0;
}



