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

  @author Simon Diepold aka. Tdotu (Universe Team) <simon.diepold@infinitycoding.de>

 */

#ifndef _mutex_h_
#define _mutex_h_

#include <stdint.h>

typedef int* mutex;

void lock(mutex m);
void unlock(mutex m);
bool try_lock(mutex m);



#endif
