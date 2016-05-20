#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void printname(int tab, char* name)
{
	int i;
	for (i = 0; i < tab; ++i)
	{
		printf ("	");
	}
	printf ("%s\n", name);
}

void create_tree(char* dirname,int tabs)
{
	DIR* dir = opendir(dirname);
	if(dir == NULL)
		return;
	struct dirent* dent;
	while((dent = readdir(dir)) != NULL)
	{
		if(dent->d_type == DT_DIR)
		{
			if(dent->d_name[0] == '.')
				continue;
			printname(tabs,dent->d_name);
			char buf[256];
			strcpy(buf, dirname);
			create_tree(strcat(strcat(buf, dent->d_name), "/"),tabs + 1);
		}

	}
	close(dir);
}




int main (int argc, char *argv[])
{
	if (argc == 1)
		create_tree("/",0);
	else
		create_tree(argv[1],0);
	return 0;
}
