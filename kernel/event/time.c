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
 *  @author Michael Sippel <micha@infinitycoding.com>
 */

#include <drivers/clock.h>
#include <event/time.h>
#include <printf.h>




const int day_to_current_month[] = {0,31,59,90,120,151,181,212,243,273,304,334}; // regeljahr
/**
 * creates a unix timestamp
 *
 * @param pointer ti time struct
 *
 * @return unix timestamp
 */
time_t unix_time(struct time *time)
{
    int year = (time->century*100)+time->year;
    int leap_years = ((year - 1) - 1968) / 4 - ((year - 1) - 1900) / 100 + ((year - 1) - 1600) / 400;
    int unix_time = time->second + (time->minute *60) + (time->hour *60*60) + ((day_to_current_month[time->month - 1] + time->day_in_month - 1) *24*60*60) + (((year-1970)*365+leap_years)*24*60*60);
    if((time->month >2) && (year%4==0 && (year%100!=0 || year%400==0)))
        unix_time += 24*60*60;
    return unix_time;
}

/**
 * Print datetime
 */
void print_time(struct time *time)
{
    char *day_string;
    switch (time->week_day)
    {
        case 0:
            day_string = "Sonntag";
            break;
        case 1:
            day_string = "Montag";
            break;
        case 2:
            day_string = "Dienstag";
            break;
        case 3:
            day_string = "Mittwoch";
            break;
        case 4:
            day_string = "Donnerstag";
            break;
        case 5:
            day_string = "Freitag";
            break;
        case 6:
            day_string = "Samstag";
            break;
    }

    printf("System Date: %02d/%02d/%02d (%s)\n", time->day_in_month, time->month, time->year, day_string);
    printf("System Time: %02d:%02d:%02d\n", time->hour, time->minute, time->second);
}


/**
 * @brief unix sys_time syscall. Returns the current unix-timestamp to the user
 * @param current user cpu state
 */
void sys_time(struct cpu_state **cpu)
{
    struct time current_time;
    update_time(&current_time);
    int stamp = unix_time(&current_time);
    (*cpu)->CPU_ARG0 = stamp;
    if((*cpu)->CPU_ARG1)
        *((int*)(*cpu)->CPU_ARG1) = stamp;
}