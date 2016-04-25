#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

unsigned char isFolder = 0x4;


void printname(int tab, char* route)
{
	int i;
	for (i = 0; i < tab; ++i)
	{
		printf ("  ");
	}
	printf ("%s\n", route);
}


void function(int tab, char* route)
{
	DIR* dir;
	struct dirent* dent;
	dir = opendir(route);
	
	if (dir == 0) return;
	while (dent = readdir(dir)) {
		if (dent->d_name[0] != '.') 
		{
			char buf[90000];
			strcpy(buf, route);
			if (dent->d_type == isFolder) 
				printname(tab, dent->d_name);
				function(tab + 1, strcat(strcat(buf, dent->d_name), "/"));
		}
	}
	closedir(dir);
}


int main (int argc, char *argv[])
{
	if (argc == 1)
		function(0, "/");
	else
		function(0, argv[1]);
	return 0;
}
