/** 
 * @file auxlib.c
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  Auxiliary functions, mainly related to string manipulation
 * 
 */



#include "auxlib.h"

char *strncpy(char *dst, const char *src, int n)
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


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: atoi                                                | *
 * |                                                                    | *
 * | DESCRIPTION: Make atoi by name                                     | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | EVENT_NAME_TYPE   EvName        X            Event name            | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE  RetCode                           | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

int atoi ( char s[] )
{
    int i    = 0;
    int n    = 0;
    n        = 0;
    
    for ( i = 0; s[i] >= '0' && s[i] <= '9'; i++ )
    {
        n = 10 * n + ( s[i] - '0' );
    }
    return ( n );
}  /*atoi*/
