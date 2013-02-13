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

#include <math.h>

/**
	Exponential-Function
**/
double exp(double x)
{
    double sigma = 0;
    int i;
    for (i = 0; i < 150; i++)
        sigma += powi(x, i ) / factorial(i);
    return sigma;
}

/**
	Natural logarithm
**/
double ln(double y)
{
    double x = (y - 1) / (y + 1);
    double sigma = 0;
    int i;
    for (i = 0; i < 150; i++)
        sigma += powi(x, 2*i+1) /(2*i+1);
    return 2*sigma;
}

/**
	Power for real exponents
**/
double pow(double base, double exponent)
{
    return exp(exponent * ln(base));
}

/**
	Sine
**/
double sin(double x)
{
	double sigma = 0;
	int i;
	for (i = 0; i < 20; i++)
		sigma += ((powi(-1, i) * powi(x, 2 * i + 1)) / factorial(2 * i + 1));
	return sigma;
}

/**
	Cosine
**/
double cos(double x)
{
    double sigma = 0;
    int i;
    for (i = 0; i < 20; i++)
        sigma += ((powi(-1, i) * powi(x, 2 * i)) / factorial(2 * i));
    return sigma;
}

/**
	Power for integer exponents
**/
double inline powi(double base, int exp)
{
    double pi = 1;
    int i;
    if (exp == 0)
    	return pi;
    for (i = 0; i < absi(exp); i++)
        pi *= base;    
    if (exp < 0)
	return 1/pi;
    else
  	eturn pi;
}

/**
	Factorial function
**/
double inline factorial(long n)
{
	double res = 1;
	int i;
	for (i = 1; i <= n; i++)
		res *= i;
	return res;
}

/**
	Absolute value
**/
double inline abs(double x)
{
	if (x < 0.0) {
		return -x;
	} else {
		return x;
	}
}

unsigned int bit_scan_forward(unsigned int i)
{
	unsigned int j;

	for (j = 0; j < 32; j++) {
		if (i & (1 << j)) {
			return j;
		}
	}
	return -1;
}
