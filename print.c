
#include<stdio.h>
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void printInt(int n)
{
   printf("%d",n);
}
void printFloat(double n)
{
   printf("%f",n);
}
void Mprint(char* str,...)
{
    va_list lst;
    va_start(lst, str);
    int len = strlen(str);
    int i = 0;
    int tempi = 0;
    double tempd = 0;
    for( i = 0 ; i<len ; i++ )
    {
	if(str[i] == '%')
	{
	   if(i+1 < len && str[i+1] == 'd')
	   {
		tempi = va_arg(lst,int);
                i++;
		printInt(tempi);
		continue;
	   } 
	   if(i+1 < len && str[i+1] == 'f')
	   {
	    tempd = va_arg(lst,double);
            i++;
	    printFloat(tempd);
	    continue;
	   }
	  putchar(str[i]);
	}
	else
	{
          putchar(str[i]);
	}
    }

}


int main (int argc, char *argv[])
{
   Mprint("%d %d %d \n",10,15,20);
   return 0;
}
