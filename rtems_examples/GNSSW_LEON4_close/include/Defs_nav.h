//*****************************************************************************
// Copyright: GMV, S.A. 2004. Property of GMV, S.A.; all rights reserved.
// Project:                  ROMA
// File:                  Defs_nav.h
// Version:                  1.9
// Date (YY/MM/DD):            2007 / 09 / 06
// Component:
// Purpose:   This file contains all the general definitions of the SRX application.
// Language:               C.
//*****************************************************************************
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//*****************************************************************************
//                        COMPILATION DEFINITIONS
//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
/* Definitions for creation of LUT files */

//*****************************************************************************
/* FFTW wisdom and planning flags for different scenarios */

#define ROMA_FFTW_WISDOM_FLAGS (FFTW_PATIENT  | FFTW_DESTROY_INPUT)
#define ROMA_FFTW_PLAN_FLAGS   (FFTW_ESTIMATE | FFTW_DESTROY_INPUT)

//*****************************************************************************
/* Definitions for outputs and traces */

//#define PRINT_SCREEN       // Print auxiliary data to screen.
//#define TRACE_FLAG         // Print extra trace data.
#define FFLUSH_FLAG          // Explicit call to print and flush.

//*****************************************************************************
/* Definitions for unused parameters (cross-platform), to remove warnings */
// UNUSED(x) - In Debug mode, issues a warning, but not in Release mode.
// UNUSED_ALWAYS(x) - Never issues a warning for the unused parameter.
// NOTE: the warning can also be disabled in MSVC++ with the "/wd4100" switch.
#ifdef _DEBUG
#define UNUSED(x)
#else
#define UNUSED(x) ((void)(x))
#endif
#define UNUSED_ALWAYS(x) ((void)(x))

//*****************************************************************************
/* Definitions for compilating a RECORD_ONLY executable */
/* Remember to remove the following libraries from the linking options:
 * libfftw3-3.lib libfftw3f-3.lib FTPlib.lib */
//#define COMPILE_RECORD_ONLY

//*****************************************************************************
//                        DEFINITIONS
//*****************************************************************************

/********************************************************************************/
/* NEW DEFINES: These defines limit the memory to be used by certain variables. */
/*    These variables in the past used dynamic memory allocation depending on   */
/*    certain configuration parameters                                          */
/********************************************************************************/
#define N_BITES_TOTAL_PACK   204600
#define CARRIER_LENGTH       5001
#define NBINS                401
#define MAXNONCOHINT         20

/********************************************************************************/
#ifdef LINUX
#define __int64 long long
#endif

/*************/
/* FILE SIZE */
/*************/
#define MAX_FILE_NAME  200               /* File name max length.*/

#ifndef _rtems_app
    #ifdef LINUX
    #define MAX_PATH_LEN   260               /* File path max length */
    #else
    #define MAX_PATH_LEN   MAX_PATH          /* File path max length.*/
    #endif
#else
#define MAX_PATH_LEN   260 
#endif
/****************************************/
/* Flags to activate outputs and traces */
/****************************************/

//#define PRINT_SCREEN       // Print auxiliary data to screen.
//#define TRACE_FLAG         // Print extra trace data.
#define ABSFILESFLAG		 /* If defined, PVT solution will be logged in AbsXXX.txt files */

/*****************/
/* BOOLEAN FLAGS */
/*****************/
#define TRUE              1
#define FALSE             0

/************************/
/* EXIT-FUNCTION VALUES */
/************************/
#define EXIT_OK           0
#define EXIT_ER          -1

/**********/
/* STATUS */
/**********/
#define INVALID           0
#define VALID             1
#define RECOVER           2
#define	RESET			  3

// Synchro&Decod Status
#define BIT_SYNC          1
#define FRAME_SYNC        2

/************************/
/* DEFINE CONSTELLATION:*/
/* -1 (GPS + GALILEO)	*/
/*  0 (GPS); 2(GALILEO) */
/************************/
#define DELTAINDEXSP3GAL 61

#define SYSFLAG	 -1

#if SYSFLAG != -1
#define NSYSTEMS  1
#else
#define NSYSTEMS  2
#endif

//#define NCORES		4
#define NCORES      3

#define NACQCHAN        1
//#define NCHANPERCORE    4
#define NUMTCKTASK       3
/*************/
/* LUT SIZES */
/*************/

#define NBINS_CODE	     21       /* Number of P and ELDiff vectors per channel.							 */
#define NBINS_CARRIER	1001       /* Maximum number of carrier bins (to set the size of the carrier LUTs).	 */

#define GPS_FRAME_SIZE   10       /* In GPS: 10 words per subframe.											 */
#define MAXNUMCHANPERCORE       7 
#define MAXNUMCHAN              MAXNUMCHANPERCORE*NUMTCKTASK		  /* Maximum number of tracking channels that the user can configure		 */
#if SYSFLAG == -1
#define GALNAVINDEX		 35		  /* Galileo index in navigation structures is defined as PRN >= 35			 */
#else
#define GALNAVINDEX       0
#endif

#define	MAXNUMSLOT		 14

#if	SYSFLAG == -1 // GPS + GALILEO

#define NPRNSLOTS		 67
#define NFREQSLOTS		 1
#define NCHIPSPERCODE	 1023
#define NCHIPSPERCODEGAL 8184

#define MAXNUMSAT                67
#define MAXNUMGPS		 32
#define MAXNUMGAL		 32
#define MAXNUMGLO		 24

#endif

#if	SYSFLAG == 0 // GPS

#define NPRNSLOTS		36
#define NFREQSLOTS		1
#define NCHIPSPERCODE	1023

#define MAXNUMSAT       36
#define MAXNUMGPS		32
#define MAXNUMGLO		24
#define MAXNUMGAL		1

#endif

#if SYSFLAG == 2 // GAL

#define NPRNSLOTS		32
#define NFREQSLOTS		1
#define NCHIPSPERCODE	8184
#define NCHIPSPERCODEGAL 8184

#define MAXNUMSAT       32
#define MAXNUMGPS		32
#define MAXNUMGLO		24
#define MAXNUMGAL		32

#endif

/***********************/
/* PI & TRIGONOMETRICS */
/***********************/
#define PI                   (3.141592653589793238)
#define HALF_PI              (1.5707963267949)
#define TWO_PI               (2*PI)
#define QUADRANT_PER_CYCLE	 (4.0)
#define RAD2DEG              (57.2957795130823)   // (180.0 / PI)
#define DEG2RAD              (0.0174532925199433) // (PI / 180.0)

//IIMP for sp3
#define MAX_SAT_PRN_INSP3_SI (211)		  /* Maximum allowed PRN for a satellite in a SP3 File */

/******************/
/* TIME CONSTANTS */
/******************/
#define SECONDS_IN_WEEK       604800.0
#define SECONDS_IN_DAY        86400.0
#define MSTOSEC               0.001
#define CODE_PERIOD			  0.001
/***********************/
/* SIZE OF BASIC TYPES */
/***********************/
#define BYTES_PER_WORD		  sizeof(int) /* 4 in a PC			*/
#define NBITS_PER_WORD        32          /* # bits in per word.*/
#define NBITS_PER_BYTE         8          /* # bits in per byte.*/

/*****************************/
/* PHYSICAL GLOBAL VARIABLES */
/*****************************/
#define LIGHTSPEED         299792458.0              /* Light speed (m/s) */
#define GRAVITY            9.80665					/* m/s^2			 */
#define EARTHRADIUS        6378137.0				/* m				 */
#define EARTHMU			   3.986004418e14			// m^3/s^2
#define OMEGAEARTH       7.2921151467e-5
#define FLATTE             (1.0/298.257223563)
#define E2                 (FLATTE*(2.0 - FLATTE))
#define E12                (E2/(1.-E2))

#define GPSL1FREQ          1575420000.0             /* L1 GPS Freq (Hz)  */
#define GPSL1LENGTH        (LIGHTSPEED / GPSL1FREQ) /* L1 Wavelength     */

#define LEAPSECS 16

/*****************************/
/*  Power of two constants   */
/*****************************/
#define TWOPOW02 4.
#define TWOPOW04 16.
#define TWOPOW05 32.
#define TWOPOW08 256.
#define TWOPOW09 512.
#define TWOPOW11 2048.
#define TWOPOW12 4096.
#define TWOPOW14 16384.
#define TWOPOW15 32768.
#define TWOPOW16 65536.
#define TWOPOW19 524288.
#define TWOPOW20 1048576.
#define TWOPOW21 2097152.
#define TWOPOW23 8388608.
#define TWOPOW24 16777216.
#define TWOPOW27 134217728.
#define TWOPOW29 536870912.
#define TWOPOW30 (TWOPOW19*TWOPOW11)
#define TWOPOW31 (TWOPOW20*TWOPOW11)
#define TWOPOW32 (TWOPOW20*TWOPOW12)
#define TWOPOW33 (TWOPOW19*TWOPOW14)
#define TWOPOW34 (TWOPOW20*TWOPOW14)
#define TWOPOW35 (TWOPOW20*TWOPOW15)
#define TWOPOW38 (TWOPOW19*TWOPOW19)
#define TWOPOW40 (TWOPOW31*TWOPOW09)
#define TWOPOW43 (TWOPOW31*TWOPOW12)
#define TWOPOW46 (TWOPOW31*TWOPOW15)
#define TWOPOW50 (TWOPOW38*TWOPOW12)
#define TWOPOW51 (TWOPOW40*TWOPOW11)
#define TWOPOW55 (TWOPOW43*TWOPOW12)
#define TWOPOW59 (TWOPOW43*TWOPOW16)

/***********/
/* GALILEO */
/***********/
#define MUEARTH_GAL			(3.986004418E14)			/* m^3/s^2 */
#define OMEGAEARTH_GAL		(7.2921151467E-5)			/* rad/s */
#define LIGHTSPEED_GAL		(299792458)					/* m/s */
#define E1_FREQ				(1575.420)					/* MHz */
#define E6_FREQ				(1278.750)					/* MHz */
#define REL_CORR_FACTOR_GAL	(-0.1331920123585475522)	/* s/m^(1/2) */
#define KEPLEREPSILON  		(1E-14)
#define MAX_KEPLER_ITER		(1000)

#define C2_MAX_32B 2147483647
#define C2_OFFSET_32B 4294967295UL //4294967296
#define C2_MAX_31B 1073741823
#define C2_OFFSET_31B 2147483648UL
#define C2_MAX_24B 8388607
#define C2_OFFSET_24B 16777216
#define C2_MAX_21B 1048575
#define C2_OFFSET_21B 2097152
#define C2_MAX_16B 32767
#define C2_OFFSET_16B 65536
#define C2_MAX_8B 127
#define C2_OFFSET_8B 256
#define C2_MAX_6B 31
#define C2_OFFSET_6B 64
#define C2_MAX_10B 511
#define C2_OFFSET_10B 1024
#define C2_MAX_11B 1023
#define C2_OFFSET_11B 2048
#define C2_MAX_12B 2047
#define C2_OFFSET_12B 4096
#define C2_MAX_13B 4095
#define C2_OFFSET_13B 8192
#define C2_MAX_14B 8191
#define C2_OFFSET_14B 16384

#define CLOCK_BIAS_SCALE_FACTOR   		(1/TWOPOW34)
#define CLOCK_DRIFT_SCALE_FACTOR   		(1/TWOPOW46)
#define CLOCK_DRIFT_RATE_SCALE_FACTOR   (1/TWOPOW59)
#define CLOCK_TOC_SCALE_FACTOR 			60
#define CLOCK_TOE_SCALE_FACTOR 			60

#define IONO_ORDER1_SCALE_FACTOR   	(1/TWOPOW02)
#define IONO_ORDER2_SCALE_FACTOR   	(1/TWOPOW08)
#define IONO_ORDER3_SCALE_FACTOR   	(1/TWOPOW15)

#define BGD_SCALE_FACTOR   			(1/TWOPOW32)
#define EPH_RAD_SCALE_FACTOR   		(1/TWOPOW31)
#define EPH_DOT_SCALE_FACTOR   		(1/TWOPOW43)
#define EPH_E_SCALE_FACTOR   		(1/TWOPOW33)
#define EPH_A12_SCALE_FACTOR  		(1/TWOPOW19)
#define EPH_CRAD_SCALE_FACTOR   	(1/TWOPOW29)
#define EPH_CMET_SCALE_FACTOR   	(1/TWOPOW05)

#define GST_UTC_A0_SCALE_FACTOR   	(1/TWOPOW30)
#define GST_UTC_A1_SCALE_FACTOR   	(1/TWOPOW50)
#define GST_UTC_T0T_SCALE_FACTOR 	3600

#define GPS_GST_A0G_SCALE_FACTOR   	(1/TWOPOW35)
#define GPS_GST_A1G_SCALE_FACTOR   	(1/TWOPOW51)
#define GPS_UTC_T0G_SCALE_FACTOR 	3600

#define ALM_TOA_SCALE_FACTOR 		600
#define ALM_A12_SCALE_FACTOR   		(1/TWOPOW09)
#define ALM_E_SCALE_FACTOR   		(1/TWOPOW16)
#define ALM_DELTAI_SCALE_FACTOR   	(1/TWOPOW14)
#define ALM_RAD_SCALE_FACTOR   		(1/TWOPOW15)
#define ALM_DOT_SCALE_FACTOR   		(1/TWOPOW33)
#define ALM_AF0_SCALE_FACTOR  		(1/TWOPOW19)
#define ALM_AF1_SCALE_FACTOR   		(1/TWOPOW38)

#define FRAC_PART_GMS_HEADER_TIME  (1/TWOPOW08)

#define NOM_GAL_SQRT_SEMIAXIS  5440.70739886 /* m^(1/2) */
#define NOM_GAL_INCLINATION    56.0          /* degrees */
#define PI_GAL 3.1415926535898

#endif
