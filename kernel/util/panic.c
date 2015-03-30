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
 *  @file /util/panic.c
 *  @brief Kernel panic dump screen module.
 *  @author Tom Slawik <tom.slawik@gmail.com>
 *  @author Simon Diepold <simon.diepold@infinitycoding.de>
 */

#include <cpu.h>
#include <drivers/cga.h>

#include <printf.h>

/**
 *  @brief Shows an errorscreen  with a error message.
 *  @param message The errormessage to be printed.
 */
void panic(char *message)
{
    char buffer[1024];
    int lines = 8;
    int len = 0;

    int i;
    len += sprintf(buffer + len, "      %c", 201);
    for(i=0; i<66; i++) len += sprintf(buffer + len, "%c", 205);
    len += sprintf(buffer + len, "%c      \n", 187);

    len += sprintf(buffer + len,
                   "      %c                             Universe                             %c      \n",
                   186, 186);

    len += sprintf(buffer + len, "      %c", 200);
    for(i=0; i<66; i++) len += sprintf(buffer + len, "%c", 205);
    len += sprintf(buffer + len, "%c      \n", 188);

    len += sprintf(buffer + len,
// 		"      %c==================================================================%c      \n"
// 		"      |                             Universe                             |      \n"
// 		"      |==================================================================|      \n"
                   "                                                                                \n"
                   "      Universe has been crashed. You have to restart your computer.             \n"
                   "                                                                                \n"
                  );

    while (*message != '\0')
    {
        buffer[len++] = *message;

        if (*message == '\n')
        {
            len += sprintf(buffer + len, "      ");
            ++lines;
        }

        ++message;
    }

    len += sprintf (buffer + len,
                    "\n      To help us improving our systems, please report this incident to us.     "
                   );

    set_color(WHITE | RED << 4);
    //clear_screen();

    //gotoxy(0, (25 / 2) - (lines / 2) - 1);
    printf("%s", buffer);

    halt();
}



/* easter egg! */
void winpanic(char *message)
{
    set_color(WHITE | BLUE << 4);
    clear_screen();

    gotoxy(37, 8);
    set_color(BLUE | LIGHT_GRAY << 4);
    printf(" Windows");
    set_color(WHITE | BLUE << 4);
    printf("\n\n      ");
    while (*message != '\0')
    {
        putchar(*message);

        if (*message == '\n')
        {
            printf("      ");
        }

        ++message;
    }
    //printf("%s\n", message);

    printf("\n\n      *  Druecken Sie eine beliebige Taste, um die Anwendung abzubrechen.\n");
    printf("      *  Druecken Sie Strg+Alt+Entf, um den Computer neu zu\n");
    printf("      starten. nicht gespeicherte Daten gehen dabei verloren.\n");
    halt();
}
