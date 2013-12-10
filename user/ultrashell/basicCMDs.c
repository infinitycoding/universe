#include <basicCMDs.h>

#include <universe.h>
#include <atoi.h>



// the echo function

int echo(int argc, char **argv)
{
	int counter;

	for(counter = 1; counter < argc; counter++)
	{
		print(argv[counter]);
		print(" ");
	}

	print("\n");

	return 0;
}


// this function prints the version of the shell

int sver(int argc, char **argv)
{
	print("Ultrashell v. 1.0.0 (debug)\n");

	return 0;
}


// from here test only

// multiplies two values and prints the result

int mul(int argc, char **argv)
{
	int result_i;
	char result_s[20];

	if(argc != 3)
	{
		print("mul: incorrect number of arguments");
		return -1;
	}

	result_i = atoi(argv[1]) * atoi(argv[2]);

	print(itoa(result_i, result_s, 10));
	print("\n");

	return result_i;
}
