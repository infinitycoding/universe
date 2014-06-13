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
 *  @author Michael Sippel <michamimosa@gmail.com>
 */

#include <drivers/cmos.h>
#include <drivers/clock.h>
#include <mm/heap.h>
#include <io.h>
#include <bcd.h>

struct cmos_data *cmos;

int timer_freq = 0;
struct time *startup;
uint64_t up_time;

void INIT_CLOCK(int freq)
{
    startup = malloc(sizeof(struct time));
    up_time = 0;
    INIT_RTC();
    INIT_PIT(freq);
}

/**
 * set PIT Fequency
 *
 * @param freqency
 * @return void
 */
void set_pit_freq(int freq)
{
    timer_freq = freq;
    int counter = 1193182 / freq;
    outb(0x40,counter & 0xFF);
    outb(0x40,counter >> 8);
}

/**
 * Initalize the Programmable Intervall Timer
 *
 * @param frequency
 * @return void
 */
void INIT_PIT(int freq)
{
    outb(0x43, 0x34);
    set_pit_freq(freq);
}

/**
 * Initalize the Real Time Clock
 *
 * @param void
 * @return void
 */
void INIT_RTC(void)
{
    cmos = malloc(sizeof(struct cmos_data));
    get_cmos_data(cmos);
    update_time(startup);
}

void sync_sys_clock()
{
    up_time += 1000/timer_freq;
}

/**
 * Updates the time from CMOS-RTC
 *
 * @param void
 */
void update_time(struct time *time)
{
    time->second =       BCD_DECODE(cmos_read_byte(0x00));
    time->alarm_sec =    BCD_DECODE(cmos_read_byte(0x01));
    time->minute =       BCD_DECODE(cmos_read_byte(0x02));
    time->alarm_min =    BCD_DECODE(cmos_read_byte(0x03));
    time->hour =         BCD_DECODE(cmos_read_byte(0x04));
    time->alarm_hour =   BCD_DECODE(cmos_read_byte(0x05));
    time->week_day =     BCD_DECODE(cmos_read_byte(0x06)) - 1;
    time->day_in_month = BCD_DECODE(cmos_read_byte(0x07));
    time->month =        BCD_DECODE(cmos_read_byte(0x08));
    time->year =         BCD_DECODE(cmos_read_byte(0x09));
    time->century =      BCD_DECODE(cmos_read_byte(0x32));
}

/**
 * Changes the Time from CMOS
 *
 * @param time New time
 */
void change_time(struct time *time)
{
    char bcd_str[2];
    cmos_write_byte(0x00, BCD_ENCODE(bcd_str, time->second));
    cmos_write_byte(0x01, BCD_ENCODE(bcd_str, time->alarm_sec));
    cmos_write_byte(0x02, BCD_ENCODE(bcd_str, time->minute));
    cmos_write_byte(0x03, BCD_ENCODE(bcd_str, time->alarm_min));
    cmos_write_byte(0x04, BCD_ENCODE(bcd_str, time->hour));
    cmos_write_byte(0x05, BCD_ENCODE(bcd_str, time->alarm_hour));
    cmos_write_byte(0x06, BCD_ENCODE(bcd_str, time->week_day));
    cmos_write_byte(0x07, BCD_ENCODE(bcd_str, time->day_in_month));
    cmos_write_byte(0x08, BCD_ENCODE(bcd_str, time->month));
    cmos_write_byte(0x09, BCD_ENCODE(bcd_str, time->year));
    cmos_write_byte(0x32, BCD_ENCODE(bcd_str, time->century));
}