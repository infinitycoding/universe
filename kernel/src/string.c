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

#include <stdint.h>


/**
 * get string lengh <= maxlen
 * @param string pointer
 * @param size_t maxlen
 * @return string lengh<= maxlen
 */
size_t strnlen(const char *s, size_t maxlen)
{
	const char *str = s;
	while (*str && maxlen) {
		++str;
		maxlen--;
	}

	return str - s;
}


/**
 * get string lengh
 * @param string pointer
 * @return string lengh
 */
size_t strlen(const char *string)
{
	size_t i = 0;
	while (string[i]) {
		i++;
	}
	
	return i;
}


/**
 * copy src string to dest
 * @param destination pointer
 * @param source string
 * @return destination string pointer
 */
char *strcpy(char *dest, const char *src)
{
	uint32_t i = 0;
	while (src[i]) {
		dest[i] = src[i];
		i++;
	}
	dest[i] = src[i];
	return dest;
}


/**
 * copy n chars from src to dest untill *src=0
 * @param destination string
 * @param source string
 * @param maximal number of chars
 * @return destination string
 */
char *strncpy(char *dest, const char *src,size_t n)
{
	uint32_t i;
	bool eos = false;
	for (i = 0; i < n; i++) {
		if (eos == false) {
			if (src[i] == NULL) {
				eos = true;
			}
			dest[i] = src[i];
		} else {
			dest[i] = NULL;
		}
	}
	return dest;
}


/**
 * copy src string behind dest string
 * @param destination string
 * @param source string
 * @return destination string
 */
char *strcat(char *dest, const char *src)
{
	size_t n = strlen(dest);
	strcpy((char*)(dest+n),src);
	return dest;
}


/**
 * copy n chars from src behind dest string untill *src=0
 * @param destination string
 * @param source string
 * @param maximal number of chars
 * @return destination string
 */
char *strncat(char *dest, const char *src, size_t n)
{
	size_t i = strlen(dest);
	strncpy((char*)(dest+i),src,n);
	return dest;
}


/**
 * compare two strings
 * @param string0
 * @param string1
 * @return 0=strings are similar;
 * @return 1=the value of the first different char of string0 is bigger
 * @return -1=the value of the first different char of string1 is bigger
 */
int strcmp(const char *st0, const char *st1)
{
	uint32_t i;
	int ret;
	ret = NULL;
	for (i = 0; st0[i] == st1[i] && st0[i]!= 0; i++);
	ret = st0[i] - st1[i];
	if (ret < 0) {
	    ret =- 1;
	}
	else if (ret > 0) {
	    ret = 1;
	}
	return ret;
}


/**
 * compare n chars of two string untill the value of str0 and str1 is not zero
 * @param string0
 * @param string1
 * @param numer of chars
 * @return 0=strings are similar;
 * @return 1=the value of the first different char of string0 is bigger
 * @return -1=the value of the first different char of string1 is bigger
 */
int strncmp(const char *st0, const char *st1, size_t n)
{
	uint32_t i;
	int ret;
	ret = NULL;
	for (i = 0; st0[i] == st1[i] && st0[i] != 0 && i < n; i++);
	ret = st0[i] - st1[i];
	if (ret < 0) {
	    ret =- 1;
	}
	else if (ret > 0) {
	    ret = 1;
	}
	return ret;
}


/**
 * search char c in str
 * @param string
 * @param searched character
 * @return pointer to the first similar char
 * @return 0=no char in str is similar to c
 */
char *strchr(const char *str, int c)
{
	int i = 0;
	while (str[i] != (char)c && str[i] != 0)
	{
	    i++;
	}
	if (str[i]) {
	    return (char*)(str+i);
	}
	return NULL;
}

/**
 * search char c in str
 * @param string
 * @param searched character
 * @return pointer to the last similar char
 * @return 0=no char in str is similar to c
 */
char *strrchr(const char *str, int c)
{
	int i = 0;
	char* ret = NULL;
	while ( str[i] != 0) {
		if (str[i] == (char)c) {
		    ret = (char*)(str+i);
		}
		i++;
	}
	return ret;
}


/**
TODO:
size_t strspn(const char* st0, const char* st1) {}
size_t strcspn(const char* st0, const char* st1) {}
char* strpbrk(const char* st0, const char* st1) {}
char* strstr(const char* st0, const char* st1) {}
char* strerror(size_t n) {}
*/

char *strtok(const char *string, char *delimiters) {
	static char *s = NULL;
	static int num_del = 0;
	if(string != NULL) {
		s = string;
		num_del = strlen(delimiters);
	} else {
		if(s == NULL) {
      			return NULL;
    		}
		string = s;
	}
	int i, j = 0;
	while(*s != '\0') {
		for(i = 0; i < num_del; i++) {
			if(*s == delimiters[i]) {
				s++;
				char *ret = (char*) malloc(j+1);
				memcpy(ret, string, j);
				ret[j] = '\0';
				return ret;
			}
		}
		s++;
		j++;
	}
		  
	char *ret = (char*) malloc(j+1);
	strcpy(ret, string);
	  
	s = NULL;
	return ret;
}

/**
 * copy memory from source to destination
 * @param destination
 * @param source
 * @param size of source area
 * @return pointer to destination area
 */
void *memcpy(void *destination, const void *source, size_t size)
{
	uint8_t *dest = destination;
	const uint8_t *sour = source;
	uint32_t i = 0;
	while (i < size) {
		dest[i] = sour[i];
		i++;
	}
	return destination;
}


/**
 * move memory from source to destination area
 * @param destination
 * @param source
 * @param size of source area
 * @return pointer to destination area
 */
void *memmove(void *destination,void *source, size_t n)
{
	char *dest = destination;
	char *src = source;
	char *c, overlap = 0;
	for (c = src; c < src + n; c++) {
		if (c == dest) {
			overlap = 1;
		}
	}
	if (overlap) {
		for (c = src + n - 1; c >= src; c--) {
			*(c + (dest - src)) = *c;
		}
	} else {
		memcpy(dest, src, n);
	}
	return destination;
}


/**
 * compare two memory areas
 * @param area0
 * @param area1
 * @param area size
 * @return 0=areas are similar
 * @return 1=the value of the first different byte of area0 is bigger
 * @return -1=the value of the first different byte of area1 is bigger
 */
int memcmp(const void *mem0, const void *mem1, size_t n)
{
	const char *st0 = mem0;
	const char *st1 = mem1;
	uint32_t i;
	int ret;
	ret = NULL;
	for (i = 0; st0[i] == st1[i] && st0[i] !=0 && i<n; i++);
	ret = st0[i] - st1[i];
	if (ret < 0) {
	    ret =- 1;
	}
	else if (ret > 0) {
	    ret = 1;
	}
	return ret;
}


/**
 * search value c in mem
 * @param memory
 * @param serched value
 * @param size of mem
 * @return pointer to the fist similar value
 * @return 0=no value in mem is similar to c
 */
void *memchr(const void *mem, int c, size_t n)
{
	uint32_t i = 0;
	const unsigned char *str = mem;
	while (str[i] != (unsigned char)c && i < n) {
	    i++;
	}
	if (str[i]) {
	    return (void*)(str+i);
	}
	return NULL;
}


/**
 * search value c in mem
 * @param memory
 * @param serched value
 * @param size of mem
 * @return pointer to the last similar value
 * @return 0=no value in mem is similar to c
 */
void *memset(void *mem, int c, size_t n)
{
	unsigned char *str = mem;
	uint32_t i;
	for (i = 0; i < n; i++) {
	    str[i]=(unsigned char)c;
	}
	return mem;
}
