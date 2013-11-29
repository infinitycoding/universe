/*
	Copyright 2012-2013 universe coding group (UCG) all rights reserved
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

#ifndef _driver_host_h_
#define _driver_host_h_

typedef enum{
    REQUEST_PORT = 1,
    REQUEST_MEMORY = 2,
    INSTALL_TRIGGER = 3,
    INSTALL_DEVICE = 4,
    UNINSTALL_DEVICE = 5
}host_signal_t;


typedef enum
{
    TRIGGER_INTERRUPT = 1,
    TRIGGER_EVENT = 2,
}host_trigger_t;


struct signal_header
{
	host_signal_t type;
	int argc;
};
// the signal header is followed by the arguments

#endif
