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
 *  @author Simon Diepold <simon.diepold@infinitycoding.de>
 */



/**
 * @brief Decodes two binary coded digits into one binary number.
 * @param digits the two BCD digits
 * @return the value of BCD coded value in binary
 */
uint8_t bcd_decode(uint8_t digits)
{
    /* a BCD byte consists of two decimal digits with 4 bit lengh
       first erasing all bits above the first digit (0b1111&digits)
       then add the second digit times 10.
     */
    return (0b1111 & digits) + (10 * (digits >> 4));
}


/**
 * @breif Encodes the binary input value into BCD format 
 * @param value the value to be encoded
 * @return the BCD encoded value
 */
uint16_t bcd_encode(uint8_t value)
{
    size_t i,b = 1;
    uint16_t digits = 0;
    for(i = 0; i < 3; i++)
    {
        uint8_t currentDigit = value%(b*10);
        value   -= currentDigit;
        digits  |= (currentDigit/b)<<(i*4);
        b*= 10;
    }
    return digits;
}