#include "auxlib.h"

char *strncpy(char *dst, const char *src, size_t n)
{
int i;
char *temp;
temp = dst; 
for (i = 0; i < n; i++)
*dst++ = *src++;
return temp;
}


char *strcpy(char *dst, const char *src)
{
char *ptr;
ptr = dst;
while(*dst++=*src++);
return(ptr);
}


int strlen(char *s)
{
char *p = s;

while (*p != '\0')
p++;
return (int)(p - s);
}




char *
strcat(char *dest, const char *src)
{
    int i,j;
    for (i = 0; dest[i] != '\0'; i++)
        ;
    for (j = 0; src[j] != '\0'; j++)
        dest[i+j] = src[j];
    dest[i+j] = '\0';
    return dest;
}



int strcmp(const char *s1, const char *s2)
{
	while((*s1 && *s2) && (*s1 == *s2))
		s1++,s2++;
	return *s1 - *s2;
}