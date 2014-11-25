#include "ls.h"
#include <list.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define RETURN_OK 0
#define RETURN_MINOR_PROBLEMS 1
#define RETURN_SERIOUS_TROUBLE 2



typedef enum ls_flags
{
	all = 0x01,
	almost_all = 0x02,
	escape = 0x04,
	ignore_backups = 0x08,
	columns = 0x10,
	help = 0x20,
	version = 0x40
}ls_flags;

void error_unknown_option(char *program, char *option)
{
		printf("%s: Unbekannte Option \"%s\"\n", program, option);
		printf("\"%s --help\" liefert weitere Informationen.", program);
}

int main(int argc, char **argv)
{
	list_t *directorys = list_create();
	unsigned int active_flags = 0;
	int i = 0;
	
	for(i = 1; i < argc; i++)
	{
		if(argv[i][0] != '-')
		{
			list_push_back(directorys, argv[i]);
		}
		else if(argv[i][1] != '-')
		{
			switch(argv[i][1])
			{
				case 'a':
					active_flags |= all;
					break;
				case 'A':
					active_flags |= almost_all;
					break;
				case 'b':
					active_flags |= escape;
					break;
				case 'B':
					active_flags |= ignore_backups;
					break;
				case 'C':
					active_flags |= columns;
					break;
				default:
					printf("%s: Ungueltige Option -- %c\n", argv[0], argv[i][1]);
					printf("\"%s --help\" liefert weitere Informationen.\n", argv[0]);
					return RETURN_SERIOUS_TROUBLE;
			}
		}
		else
		{
			switch(argv[i][2])
			{
				case 'h':
					if(strcmp(argv[i], "--help") == 0)
					{
						active_flags |= help;
					}
					else
					{
						error_unknown_option(argv[0], argv[i]);
						return RETURN_SERIOUS_TROUBLE;
					}
				case 'v':
					if(strcmp(argv[i], "--version") == 0)
					{
						active_flags |= version;
					}
					else
					{
						error_unknown_option(argv[0], argv[i]);
						return RETURN_SERIOUS_TROUBLE;
					}
					break;
				default:
					error_unknown_option(argv[0], argv[i]);
					return RETURN_SERIOUS_TROUBLE;
			}
		}
	}
	
	if(active_flags & help)
	{
		printf("Hilfe ist noch nicht vorhanden.\n");
		
		return RETURN_OK;
	}
	else if(active_flags & version)
	{
		printf("ls (Universe coreutils) 1.00\n");
		printf("Copyright (C) 2014 Infinitycoding\n");
		printf("Lizenz GPLv3+: GNU GPL Version 3 oder hoeher <http://gnu.org/licenses/gpl.html>\n");
		printf("Dies ist freie Software: Sie koennen sie aendern und weitergeben.\n");
		printf("Es gibt keinerlei Garantien, soweit wie es das Gesetz erlaubt.\n");
		printf("\n");
		printf("Geschrieben von Peter Hoesch\n");
		return RETURN_OK;
	}
	
	char cwd[256];
	
	if(getcwd(cwd, 256) == NULL)
		return RETURN_SERIOUS_TROUBLE;
	
	int fdesc = open(cwd, O_RDONLY, 0);
	dirent_t *dir = malloc(sizeof(dirent_t));
	
	do
	{	
		dir = readdir(fdesc, dir);
		
		if(dir != NULL)
		{
			if(dir->name[0] == '.')
			{
				if((dir->name[1] == '\0' || (dir->name[1] == '.' && dir->name[2] == '\0')) && !(active_flags & all))
					continue;
				else if(!(active_flags & all) || !(active_flags & almost_all))
					continue;
			}
			
			printf("%s ", dir->name);
		}
	}while(dir != NULL);
	
	printf("\n");
	
	return RETURN_OK;
}
