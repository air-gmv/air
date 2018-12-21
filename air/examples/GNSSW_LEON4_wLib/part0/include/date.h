//*****************************************************************************
// Copyright: GMV, S.A. 2004. Property of GMV, S.A.; all rights reserved.
// Project:                  ROMA
// File:                  date.h
// Version:                  1.1
// Date (YY/MM/DD):            2008 / 02 / 21
// Component:
// Purpose:   Functions to compute and convert date and time format.
// Language:               C.
//
// History:
//   1.0   -   2009/04/20   -   JJNC   -   R1.0_FUN_LINUX
//   1.1   -   2009/10/01   -   JJNC   -   R1.14_FUN_USB_AND_GENERAL
//*****************************************************************************
#ifndef DATE_H
#define DATE_H



//*****************************************************************************
//                        FUNCTIONS INTERFACE
//*****************************************************************************
int    GL_GPSToCal( int week, double tow, int *year, int *month, int *day,
                              int *hour, int *minute, double *second);
void   convertDateToWN( struct  tm *gmtTime, int *wn);
void   convertDateToTOW( struct  tm *gmtTime, double *tow);
int    GL_CheckDate( int *year, int *month, int *day, int *hour, int *min, double *sec );
int    getTimeStr(char *pchTimeStr);
int    dateToDay(int year, int month, int day);
int    leapYear(int year);
double julianDay( int year, int month, double floatday);
void   calendarToGps( int year, int month, int day, int hour, int min, double sec, long *wn, double *tow);
int    GL_DateToDay( int year,   int month, int day, int * yday);
void   TimeOfDay2cal(double ToD, int *hour, int *min, double *sec);
int	   GetMonthNum(char *szMonth);

#endif
