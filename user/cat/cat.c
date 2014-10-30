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

#include <unistd.h>
#include <stdio.h>
#include <stddef.h>

int main(int argc, char **argv) {
  if(argc < 2) {
    printf("Usage: %s [file 1] [file 2] ...\n", argv[0]);
    return -1;
  }
  
  int i;
  struct stat filestat;
  for(i = 1; i < argc; i++) {
    int f = open(argv[i], O_RDONLY, 0);
    if(f >= 0) {
      fstat(f, &filestat);
      int len = filestat.st_size;
      char c;
      while(len > 0) {
        read(f, &c, 1);
        printf("%c", c);
        len--;
      }
      
      close(f);
    } else {
      printf("File not Found(%d, \"%s\")!\n", i, argv[i]);
    }
  }
  
  return 0;
}

