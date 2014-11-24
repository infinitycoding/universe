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
 */

#include <mutex.h>

void lock(mutex *m)
{
    while(!try_lock(m));
}

void unlock(mutex *m)
{
    *m = false;
}


bool try_lock(mutex *m)
{
    int r;
    asm(
        "mov $1, %%eax;"
        "xchg %%eax,(%1);"
        : "=a"(r) : "D" (m)
    );
    return !r;
}
