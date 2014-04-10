/*
     Copyright 2014 Infinitycoding all rights reserved
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



#include <basicCMDs.h>

#include <universe.h>
#include <unistd.h>
#include <atoi.h>
#include <stdio.h>



// this isn't the function you are looking for

int dummy(int argc, char **argv)
{
	printf("This isn't the function you are looking for.\n");	

	return 0;
}


// the echo function

int echo(int argc, char **argv)
{
	int counter;

	for(counter = 1; counter < argc; counter++)
	{
		printf(argv[counter]);
		printf(" ");
	}

	printf("\n");

	return 0;
}


// this function prints the version of the shell

int sver(int argc, char **argv)
{
	printf("Ultrashell v. 1.1.0 (debug)\n");

	return 0;
}


// this function always returns true

int cmdtrue(int argc, char **argv)
{
	return 0;
}


// this function always returns false

int cmdfalse(int argc, char **argv)
{
	return 1;
}


// changes the working directory

int cd(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("cd: incorrect number of arguments\n");
		return 1;
	}

	if(chdir(argv[1]) == -1)
	{
		printf("cd: %s: unable to find file or directory", argv[1]);
		return 2;
	}

	return 0;
}


// printf the current working directory

int pwd(int argc, char **argv)
{
	if(argc != 1)
	{
		printf("pwd: incorrect number of arguments\n");
		return 1;
	}

	char wd[80];

	if(getcwd(wd, 80) == NULL)
	{
		printf("pwd: some error has occured during calling getcwd\n");
		return 2;
	}

	printf("%s\n", wd);

	return 0;	
}


// from here test only

// adds two values and prints the result

int add(int argc, char **argv)
{
	int result_i;
	char result_s[20];

	if(argc != 3)
	{
		printf("add: incorrect number of arguments\n");
		return 1;
	}

	result_i = atoi(argv[1]) + atoi(argv[2]);

	printf(itoa(result_i, result_s, 10));
	printf("\n");

	return result_i;
}


// subtracts two values and prints the result

int sub(int argc, char **argv)
{
	int result_i;
	char result_s[20];

	if(argc != 3)
	{
		printf("sub: incorrect number of arguments\n");
		return 1;
	}

	result_i = atoi(argv[1]) - atoi(argv[2]);

	printf(itoa(result_i, result_s, 10));
	printf("\n");

	return result_i;
}


// multiplies two values and prints the result

int mul(int argc, char **argv)
{
	int result_i;
	char result_s[20];

	if(argc != 3)
	{
		printf("mul: incorrect number of arguments\n");
		return 1;
	}

	result_i = atoi(argv[1]) * atoi(argv[2]);

	printf(itoa(result_i, result_s, 10));
	printf("\n");

	return result_i;
}


// divides two values and prints the result

int div(int argc, char **argv)
{
	int result_i;
	char result_s[20];

	if(argc != 3)
	{
		printf("div: incorrect number of arguments\n");
		return 1;
	}

	if(atoi(argv[2]) == 0)
	{
		printf("div: divide by zero\n");
		return 2;
	}

	result_i = atoi(argv[1]) / atoi(argv[2]);

	printf(itoa(result_i, result_s, 10));
	printf("\n");

	return result_i;
}
