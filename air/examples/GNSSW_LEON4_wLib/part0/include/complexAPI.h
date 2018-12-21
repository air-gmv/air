//*****************************************************************************
// Copyright: GMV, S.A. 2004. Property of GMV, S.A.; all rights reserved.
// Project:                  ROMA
// File:                  complexAPI.h
// Version:                  1.1
// Date (YY/MM/DD):            2007 / 12 / 13
// Component:
// Purpose:   this header file contais the header for complex operations.
// Language:               C.
//
// History:
//   1.0   -   2009/04/20   -   JJNC   -   R1.0_FUN_LINUX
//   1.1   -   2009/04/28   -   JJNC   -   R1.1_PFM_SINGLE_FFTW
//*****************************************************************************
#ifndef COMPLEX_H
#define COMPLEX_H

#include <math.h>

struct complex_t
{
   float  x;
   float  y;
};



//*****************************************************************************
//                        MACROS
//*****************************************************************************
#define cmult( a, b, c)                          \
{                                                \
   c.x = ((a.x) * (b.x)) - ((a.y) * (b.y));      \
   c.y = ((a.x) * (b.y)) + ((a.y) * (b.x));      \
}



//*****************************************************************************
//                        FUNCTIONS INTERFACE
//*****************************************************************************
struct complex_t csum(struct complex_t a, struct complex_t b);
float            absolute(float value);

#endif
