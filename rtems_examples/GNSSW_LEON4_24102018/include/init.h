//*****************************************************************************
// Copyright: GMV, S.A. 2004. Property of GMV, S.A.; all rights reserved.
// Project:                  ROMA
// File:                  init.h
// Version:                  1.3
// Date (YY/MM/DD):            2008 / 04 / 08
// Component:
// Purpose:   this file contains the initialization functions of the ROMA applciation.
// Language:               C.
//
// History:
//   1.0   -   2009/04/20   -   JJNC   -   R1.0_FUN_LINUX
//   1.1   -   2009/10/01   -   JJNC   -   R1.14_FUN_USB_AND_GENERAL
//   1.2   -   2009/10/20   -   JJNC   -   R1.15_FUN_CENTER_FREQ_LOW_ELEV
//   1.3   -   2009/10/26   -   JJNC   -   R1.15_FUN_CENTER_FREQ_HIGH_ELEV_VIRTUAL_TCK
//*****************************************************************************
#ifndef INIT_H
#define INIT_H

#include <siria.h>
#include <interface.h>
//*****************************************************************************
//                        FUNCTIONS INTERFACE
//*****************************************************************************
int initialize(char *configFileName);
void finalize(void);

#endif
