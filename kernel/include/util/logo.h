#ifndef _LOGO_H_
#define _LOGO_H_

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
 *  @author Tom Slawik <tom.slawik@gmail.com>
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <drivers/cga.h>

static const char *__logo = "\
                                                                               \n\
           UUUUUUUU     UUUUUUUU NNNNNNNN        NNNNNNNN IIIIIIIIII           \n\
           U::::::U     U::::::U N:::::::N       N::::::N I::::::::I           \n\
           U::::::U     U::::::U N::::::::N      N::::::N I::::::::I           \n\
           UU:::::U     U:::::UU N:::::::::N     N::::::N II::::::II           \n\
            U:::::U     U:::::U  N::::::::::N    N::::::N   I::::I             \n\
            U:::::D     D:::::U  N:::::::::::N   N::::::N   I::::I             \n\
            U:::::D     D:::::U  N:::::::N::::N  N::::::N   I::::I             \n\
            U:::::D     D:::::U  N::::::N N::::N N::::::N   I::::I             \n\
            U:::::D     D:::::U  N::::::N  N::::N:::::::N   I::::I             \n\
            U:::::D     D:::::U  N::::::N   N:::::::::::N   I::::I             \n\
            U:::::D     D:::::U  N::::::N    N::::::::::N   I::::I             \n\
            U::::::U   U::::::U  N::::::N     N:::::::::N   I::::I             \n\
            U:::::::UUU:::::::U  N::::::N      N::::::::N II::::::II           \n\
             UU:::::::::::::UU   N::::::N       N:::::::N I::::::::I           \n\
               UU:::::::::UU     N::::::N        N::::::N I::::::::I           \n\
                 UUUUUUUUU       NNNNNNNN         NNNNNNN IIIIIIIIII           \n\
                                                                               \n\
";

static void print_logo(color_t color)
{
    color_t color_saved = get_color();

    set_color(color | BLACK << 4);

    puts(__logo);

    set_color(color_saved);
}

#endif
