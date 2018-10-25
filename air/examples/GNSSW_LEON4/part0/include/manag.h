//*********************************************************************************
// Copyright: GMV, S.A. 2004. Property of GMV, S.A.; all rights reserved.
// Project:                  ROMA
// File:                  manag.h
// Version:                  1.7
// Date (YY/MM/DD):            2007 / 02 / 14
// Component:
// Purpose:
// Language:               C.
//
// History:
//   1.0   -   2009/04/20   -   JJNC   -   R1.0_FUN_LINUX
//   1.1   -   2009/07/02   -   JJNC   -   R1.6_PFM_ACQUISITION_PER_FREQ
//   1.2   -   2009/09/07   -   JJNC   -   R1.11_FUN_VIRTUAL_TRACKING_INIT
//   1.3   -   2009/09/07   -   JJNC   -   R1.12_PFM_TRACE_AND_FFLUSH
//   1.4   -   2009/10/01   -   JJNC   -   R1.14_FUN_USB_AND_GENERAL
//   1.5   -   2009/10/20   -   JJNC   -   R1.15_FUN_CENTER_FREQ_LOW_ELEV
//   1.6   -   2009/10/26   -   JJNC   -   R1.15_FUN_CENTER_FREQ_HIGH_ELEV_VIRTUAL_TCK
//   1.7   -   2009/11/03   -   JJNC   -   R.16_FUN_GENERAL_AND_ARM
//*********************************************************************************
#ifndef MANAG_H
#define MANAG_H

#include "siria.h"
//*********************************************************************************
//                        DEFINITIONS
//*********************************************************************************

//*********************************************************************************
//                        STRUCTURES DEFINITION
//*********************************************************************************

//*********************************************************************************
//                        FUNCTIONS INTERFACE
//*********************************************************************************
void ManageLoopTracking(T_Channel *chData_P, int CurrentCpu, int nchan);

void ReceiverProcessing(void);

#endif
