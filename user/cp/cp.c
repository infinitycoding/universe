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

int main(int argc, char **argv) {
  if(argc < 3) {
    printf("Usage: %s [source] [destination]\n", argv[0]);
    return -1;
  }
  
  int src = open(argv[1], O_RDONLY, 0);
  int dest = open(argv[2], O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
  if(src < 0 || dest < 0) {
    printf("Error.\n");
    return -1;
  }
  
  struct stat filestat;
  fstat(src, &filestat);
  int len = filestat.st_size;
  char i = 0;
  
  while(len > 0) {
    read(src, &i, 1);
    write(dest, &i, 1);
    len--;
  }
  
  close(src);
  close(dest);  
  
  return 0;
}

