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
 **/
#include <stdint.h>
#include <printf.h>
#include <cpu.h>
#include <paging.h>


/**
 * Mapping Dump
 * Only works wehne every PD is mapped in in kernelspace Area
 * @param pointer to the phys. pd frame
 * @return void
 **/

void pd_dump(uint32_t *pd){
    pd += 0xC0000000;
    int pdi, pti, phys_base, virt_base, size, sec = 0;
    uint32_t *pt;
    for( pdi = 0; pdi < PD_LENGTH; pdi++ ){
        if(sec && (pd[pdi] & 1) == 0){
            printf("%#010X - %#010X => %#010X - %#010X\n", phys_base, phys_base + (4096 * size), virt_base, virt_base + (4096 * size));
            sec = 0;
        }

        while( (pd[pdi] & 1) == 0 ){
            pdi++;
        }

        pt = (uint32_t*) ( ( (pd[pdi] | 0xfff) ^ 0xfff) + 0xC0000000);

        for( pti = 0; pti < PT_LENGTH; pti++ ){
            if(sec && (pt[pti] & 1) == 0){
                printf("%#010X - %#010X => %#010X - %#010X\n", phys_base, phys_base + (4096 * size), virt_base, virt_base + (4096 * size));
                sec = 0;
            }

            while( (pt[pti] & 1) == 0 ){
                pti++;
            }
            if(sec == 0){
                sec = 1;
                phys_base = ((pt[pti] | 0xfff) ^ 0xfff);
                virt_base = (pdi << 22) | (pti << 12);
                size = 0;

            }else{
                if( (phys_base + (4096 * (size+1) )) == ( (pt[pti] | 0xfff) ^ 0xfff) ){
                    size++;
                }else{
                    printf("%#010X - %#010X => %#010X - %#010X\n", phys_base, phys_base + (4096 * size), virt_base, virt_base + (4096 * size));
                    phys_base = ((pt[pti] | 0xfff) ^ 0xfff);
                    virt_base = (pdi << 22) | (pti << 12);
                    size = 0;
                }

            }
        }

    }

    if(sec){
        printf("%#010X - %#010X => %#010X - %#010X\n", phys_base, phys_base + (4096 * size), virt_base, virt_base + (4096 * size));
    }
}
