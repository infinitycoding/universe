#ifndef STDIO_H
#define STDIO_H
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

#include <stdint.h>
#include <stdarg.h>

#define stdin 0
#define stdout 1
#define stderr 2

#define STDIN 0
#define STDOUT 1
#define STDERR 2

// printf
int printf(const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);
int getchar(void);

// files
typedef struct file {
	char *fpos;
	void *base;
	uint8_t handle;
	int8_t flags;
	int8_t unget;
	unsigned long alloc;
	uint8_t buffincrement;
} file_t;
typedef struct file FILE;

FILE *fopen(const char *path, char *modus);
int fclose(FILE *file);
size_t fread(void *buf, size_t size, size_t n, FILE *file);
size_t fwrite(void *buf, size_t size, size_t n, FILE *file);
void fseek(FILE *file, int off, int whence);
int ftell(FILE *file);

#endif

