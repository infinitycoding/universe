#ifndef	_string_h_
#define	_string_h_
#include "stdint.h"

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

extern size_t strnlen(const char *s, size_t maxlen);
extern size_t strlen(const char*);
extern char* strcpy(char*,const char*);
extern char* strncpy(char*,const char*,size_t);
extern char* strcat(char*,const char*);
extern char* strncat(char*,const char*,size_t);
extern int strcmp(const char*,const char*);
extern int strncmp(const char*,const char*,size_t);
extern char* strchr(const char*,int);
extern char* strrchr(const char*,int);

//extern size_t strspn(const char*,const char*);
//extern size_t strcspn(const char*,const char*);
//extern char* strpbrk(const char*,const char*);
//extern char* strstr(const char*,const char*);
//extern char* strerror(size_t);

extern char* strtok(char*,const char*);

extern void* memcpy(void*,const void*,size_t);
extern void* memmove(void*,const void*,size_t);
extern int memcmp(const void*,const void*,size_t);
extern void* memchr(const void*,int,size_t);
extern void* memset(void*,int,size_t);

extern void strev(char* output,char* input);
extern void itodez(int zahl,char* output);
extern void uitodez(uint32_t zahl,char* output);
extern void itohex(int zahl,char* output);
extern void uitohex(uint32_t zahl,char* output);

#endif
