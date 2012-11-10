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
**/

/**
  @author Peter Hösch <phoesch@gmx.de>
*/

#include <stdint.h>
#include <string.h>

static char *param;
static int comnumber=2;
static char *coms[256] = {"var", "help", "reboot"};
static char compointer[256];

int install_cmd(char *name, char pointer){
    int temp = 0;
    int failture;
    while(!strcmp(*name, *coms[temp])){ // compares new command to old commands
        temp++;
        if(temp > comnumber){
            break;
        }
    }
    if(temp < comnumber){   // if this command is existing the loop didn't ran more often than there are commands
        printf("This command exists already.\n");
        failture = 1;
    }else{   // new command is added to array
        comnumber++;
        *coms[comnumber] = *name;
        compointer[comnumber] = pointer;
        failture = 0;
    }
    return failture;   // you can check if the command exists or not
}

unsigned int comlength(char *comandparam){
    unsigned int counter = 0;
    while(comandparam[counter] =! ' '){
        counter++;
    }
    return counter;
}

int analyse(char *in){
    unsigned int length = comlength(in);   // Cut the string before the parameters
    length++;
    *param = *(in + length);   // command can read out his parameters
    int ret;
    int counter = 0;
    int fail = 0;
    while(!strcmp(*in, coms[counter])){   // look up which command the user entered
        counter++;
        if(comnumber < counter){
            fail = 1;
            break;
        }
    }
    if(!fail){   // the comand exists
        ret = 0;
        //goto compointer[counter];
    }else{   // the command exists not
        printf("Unknown command or file.\n");
        ret = 1;
    }
    return ret;   // you can check if the command exists or not
}
