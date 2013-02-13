#ifndef _math_h_
#define _math_h_

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

	Includes important mathematical functions such as sine or cosine.
	Computation uses Taylor series.

	@author Tom Slawik <tom.slawik@gmail.com>
	@author Adrian Jablonski <support@quadsoft.org>
*/

#define PI = 3.1415926535897931
#define E  = 2.7182818284590451

double exp(double x);
double ln(double y);
double pow(double base, double exp);
double sin(double x);
double cos(double x);

int powi(int base, int exponent);
double inline factorial(long n);
double inline abs(double x);

unsigned int bit_scan_forward(unsigned int);

#endif