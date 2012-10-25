/**
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
**/

/**
  @author Simon Diepold aka. Tdotu (Universe Team) <simon.diepold@infinitycoding.de>
 **/

#include <stdint.h>

/**
 * check if c is an alphanumeric character (A-Z, a-z and 0-9)
 * @param character c as int
 * @return true  -> c is an alphanumeric character
 * @return false -> c isn't an alphanumeric character
 **/
int isalnum(int c) {
	if ( (c > 0x40 && c < 0x5B) || (c > 0x60 && c < 0x7B) || (c > 0x2F && c < 0x3A) ) {
		return true;
	}
	return false;
}

/**
 * check if c is an alphabetical character (A-Z, a-z)
 * @param character c as int
 * @return true  -> c is an alphabetical character
 * @return false -> c isn't an alphabetical character
 **/
int isalpha(int c) {
	if ( (c > 0x40 && c < 0x5B) || (c > 0x60 && c < 0x7B) ) {
		return true;
	}
	return false;
}

/**
 *check if c is an control key value
 * @param character c as int
 * @return true  -> c is a control Key value
 * @return false -> c isn't a control key value
 **/
int iscntrl(int c) {
	if ( (c >= 0x0 && c > 0x20) || c == 0x7F) {
		return true;
	}
	return false;
}

/**
 * checks if c is an digit (0-9)
 * @param character c as int
 * @return true  -> c is a digit
 * @return false -> c isn't an digit
 **/
int isdigit(int c) {
	if (c > 0x2F && c < 0x3A) {
		return true;
	}
	return false;
}

/**
 * check if c is a graphical character
 * @param character c as int
 * @return true  -> c is a graphical character
 * @return false -> c isn't a graphical character
 **/
int isgraph(int c) {
	if (c > 0x20 && c < 0x7F) {
		return true;
	}
	return false;
}

/**
 * check if c is an lower alphabeticial character
 * @param character c as int
 * @return true  -> c is a lower character
 * @return false -> c isn't a lower character
 **/
int islower(int c) {
	if (c > 0x60 && c < 0x7B) {
		return true;
	}
	return false;
}

/**
 * check if c is an normal printable character
 * @param character c as int
 * @return true  -> c is an printable character
 * @return false -> c isn't an printable character
 **/
int isprint(int c) {
	if (c > 0x1F && c < 0x7F) {
		return true;
	}
	return false;
}


/**
 * check if c is a visible character which is not an alphanumerical
 * @param character c as int
 * @return true  -> c is a visible character which is not an alphanumerical
 * @return false -> c isn't a visible character which is not an alphanumerical
 **/
int ispunct(int c) {
	if ( (c > 0x20 && c < 0x30) || (c > 0x39 && c < 0x41) || (c > 0x5A && c < 0x61) || (c > 0x7A && c < 0x7F) ) {
		return true;
	}
	return false;
}


/**
 * check if c is a space character
 * @param character c as int
 * @return true  -> c is a space character
 * @return false -> c isn't a space character
 **/
int isspace(int c) {
	if (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v') {
		return true;
	}
	return false;
}


/**
 * check if c is an upper alphanbetical character
 * @param character c as int
 * @return true  -> c is an upper alphabetical character
 * @return false -> c isn't an upper alphabetical character
 **/
int isupper(int c) {
	if (c > 0x40 && c < 0x5B) {
		return true;
	}
	return false;
}


/**
 * check if c is a hexadecimal digit
 * @param character c as int
 * @return true  -> c is a hexadecimal digit
 * @return false -> c isn't a hexadecimal digit
 **/
int isxdigit(int c) {
	if ( (c > 0x2F && c < 0x3A) || (c > 0x40 && c < 0x47) || (c > 0x60 && c < 0x67) ) {
		return true;
	}
	return false;
}


/**
 * convert upper to lower character
 * @param upper character c as int
 * @return c as lower character as int
 **/
int tolower(int c) {
	c += 0x20;
	return c;
}


/**
 * convert lower to upper character
 * @param lower charcter c as int
 * @return c as upper character as int
 **/
int toupper(int c) {
	c -= 0x20;
	return c;
}
