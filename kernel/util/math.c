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
 *  @author Adrian Jablonski <support@quadsoft.org>
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
int powi(int base, int exponent)
{
  int result = base;
  int i;

  if (exponent == 0) {
    result = 1;
  } else {
    for (i = 0; i < exponent - 1; ++i) {
      result *= base;
    }
  }

  return result;
}

/**
	Factorial function
**/
double factorial(long n)
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
int abs(int x)
{
	if (x < 0) {
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
