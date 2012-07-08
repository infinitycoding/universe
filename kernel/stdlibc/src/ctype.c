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

	
	
    Diese Datei ist ein Teil vom Universe Kernel.

    Das Universe Kernel ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder jeder späteren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Das Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEWÄHELEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License für weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>

int isalnum(int c){
	if((c>0x40 && c<0x5B) || (c>0x60 && c<0x7B) ||(c>0x2F && c<0x3A) ){
		return true;
	}
	return false;
}

int isalpha(int c){
	if((c>0x40 && c<0x5B) || (c>0x60 && c<0x7B)){
		return true;
	}
	return false;
}

int iscntrl(int c){
	if((c>=0x0 && c>0x20)|| c==0x7F){
		return true;
	}
	return false;
}

int isdigit(int c){
	if(c>0x2F && c<0x3A){
		return true;
	}
	return false;
}

int isgraph(int c){
	if(c>0x20 && c<0x7F){
		return true;
	}
	return false;
}

int islower(int c){
	if(c>0x60 && c<0x7B){
		return true;
	}
	return false;
}

int isprint(int c){
	if(c>0x1F && c<0x7F){
		return true;
	}
	return false;
}

int ispunct(int c){
	if((c>0x20 && c<0x30) || (c>0x39 && c<0x41) || (c>0x5A && c<0x61) || (c>0x7A && c<0x7F)){
		return true;
	}
	return false;
}

int isspace(int c){
	if(c==' ' || c=='\f' || c=='\n' || c=='\r' || c=='\t' || c=='\v'){
		return true;
	}
	return false;
}

int isupper(int c){
	if(c>0x40 && c<0x5B){
		return true;
	}
	return false;
}

int isxdigit(int c){
	if((c>0x2F && c<0x3A) || (c>0x40 && c<0x47) || (c>0x60 && c<0x67)){
		return true;
	}
	return false;
}

int tolower(int c){
	c+=0x20;
	return c;
}

int toupper(int c){
	c-=0x20;
	return c;
}