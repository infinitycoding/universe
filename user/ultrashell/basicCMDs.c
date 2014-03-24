#include <basicCMDs.h>

#include <universe.h>
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
