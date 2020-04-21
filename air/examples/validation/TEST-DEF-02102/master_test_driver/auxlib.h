/** 
 * @file auxlib.h
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  auxlib.h function prototypes. Mainly related to 
 * 		string manipulation 
 * 
 */



#ifndef _AUXLIB_H_
#define _AUXLIB_H_


int strlen(char *s);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dst, const char *src);
char * strcat(char *dest, const char *src);
char *strncpy(char *dst, const char *src, int n);
int atoi ( char s[] );

#endif
