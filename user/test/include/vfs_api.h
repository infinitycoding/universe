#ifndef _VFS_API_H_
#define _VFS_API_H_

/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of Universe.
 
     Universe is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.
 
     Universe is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
 
     You should have received a copy of the GNU General Public License
     along with Universe. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <stdbool.h>
bool test_vfs_api(void);
bool test_create_file(const char *name);
bool test_read_write_file(const char *name);
bool test_apped_file(const char *name);
bool test_fifo(char *name);
#endif