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

size_t strlen(const char* string){
	size_t i=0;
	while(string[i]){i++;}
	return i;
}

char* strcpy(char* dest, const char* src){
	uint32_t i=0;
	while(src[i]){
		dest[i]=src[i];
		i++;
	}
	dest[i]=src[i];
	return dest;
}

char* strncpy(char* dest, const char* src,size_t n){
	uint32_t i;
	bool eos=false;
	for(i=0;i<n;i++){
		if(eos==false){
			if(src[i]==NULL){eos=true;}
			dest[i]=src[i];
		}else{
			dest[i]=NULL;
		}
	}
	return dest;
}

char* strcat(char* dest, const char* src){
	size_t n=strlen(dest);
	strcpy((char*)(dest+n),src);
	return dest;
}

char* strncat(char* dest, const char* src, size_t n){
	size_t i=strlen(dest);
	strncpy((char*)(dest+i),src,n);
	return dest;
}

int strcmp(const char* st0, const char* st1){
	uint32_t i;
	int ret;
	ret=NULL;
	for (i=0; st0[i] == st1[i] && st0[i]!= 0; i++);
	ret=st0[i]-st1[i];
	if(ret<0){ret=-1;}
	else if(ret>0){ret=1;}
	return ret;
}

int strncmp(const char* st0, const char* st1, size_t n){
	uint32_t i;
	int ret;
	ret=NULL;
	for (i=0; st0[i] == st1[i] && st0[i] !=0 && i<n; i++);
	ret=st0[i]-st1[i];
	if(ret<0){ret=-1;}
	else if(ret>0){ret=1;}
	return ret;
}

char* strchr(const char* str, int c){
	int i=0;
	while(str[i]!=(char)c && str[i]!=0){i++;}
	if(str[i]){return (char*)(str+i);}
	return NULL;
}

char* strrchr(const char* str, int c){
	int i=0;
	char* ret=NULL;
	while( str[i]!=0){
		if(str[i]==(char)c){ret=(char*)(str+i);}
		i++;
	}
	return ret;
}

/*
size_t strspn(const char* st0, const char* st1){}
size_t strcspn(const char* st0, const char* st1){}
char* strpbrk(const char* st0, const char* st1){}
char* strstr(const char* st0, const char* st1){}
char* strerror(size_t n){}
char* strtok(char* st0, const char* st1){}
*/

void* memcpy(void* destination, const void* source, size_t size){
	uint8_t* dest=destination;
	uint8_t* sour=source;
	uint32_t i=0;
	while(i<size){
		dest[i]=sour[i];
		i++;
	}
	return destination;
}

void* memmove(void* destination, const void* source, size_t n){
	char* dest=destination;
	char* src=source;
    char* c, overlap = 0;
    for (c = src; c < src + n; c++){
        if (c == dest){
            overlap = 1;
		}
	}
    if (overlap){              
        for (c = src + n - 1; c >= src; c--){
            *(c + (dest - src)) = *c;
		}
    }else{
        memcpy(dest, src, n);
	}
	return destination;
}

int memcmp(const void* mem0, const void* mem1, size_t n){
	const char* st0=mem0;
	const char* st1=mem1;
	uint32_t i;
	int ret;
	ret=NULL;
	for (i=0; st0[i] == st1[i] && st0[i] !=0 && i<n; i++);
	ret=st0[i]-st1[i];
	if(ret<0){ret=-1;}
	else if(ret>0){ret=1;}
	return ret;
}

void* memchr(const void* mem, int c, size_t n){
	uint32_t i=0;
	const unsigned char* str=mem;
	while(str[i]!=(unsigned char)c && i<n){i++;}
	if(str[i]){return (void*)(str+i);}
	return NULL;
}

void* memset(void* mem, int c, size_t n){
	unsigned char* str=mem;
	uint32_t i;
	for(i=0;i<n;i++){str[i]=(unsigned char)c;}
	return mem;
}

