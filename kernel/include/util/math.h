#ifndef _MATH_H_
#define _MATH_H_

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
 *	@author Adrian Jablonski <support@quadsoft.org>
 */

#define PI = 3.1415926535897931
#define E  = 2.7182818284590451

double exp(double x);
double ln(double y);
double pow(double base, double exp);
double sin(double x);
double cos(double x);

int powi(int base, int exponent);
double factorial(long n);
int abs(int x);

unsigned int bit_scan_forward(unsigned int);

#endif
