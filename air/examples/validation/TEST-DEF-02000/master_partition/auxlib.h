#ifndef _AUXLIB_H_
#define _AUXLIB_H_

#define __SIZE_TYPE__ long unsigned int
typedef __SIZE_TYPE__ size_t;

int strlen(char *s);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dst, const char *src);
char * strcat(char *dest, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
int atoi ( char s[] );

#endif
