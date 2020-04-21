/*------------------------------------------------------------------------+
 * +xxxxxxxxxxxxxxxxxxxxxx SKYSOFT PORTUGAL SA xxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * | LEGAL NOTICE: The source code contained herein is not intended to  | *
 * | allow the development of source code or software for use or        | *
 * | distribution. This file contains SKYSOFT PORTUGAL, SA proprietary  | *
 * | information and is supplied on the condition that it shall solely  | *
 * | be used within the terms of the AVT License Agreement. Except as   | *
 * | otherwise provided thereof, you may not,without prior written      | *
 * | permission from SKYSOFT PORTUGAL, S.A. provide, or otherwise       | *
 * | disclose information regarding any discovered bugs, errors,        | *
 * | architecture issues or problems with the Source Code or Compiled   | *
 * | Code to any other party.                                           | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *                                                                        *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * | FILE: AVT_TestTool.h                                               | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVT_TESTTOOL                                            | *
 * | UNIT NAME: AVT                                                     | *
 * |                                                                    | *
 * | DESCRIPTION: Header file with definitions used by all the modules. | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   17/09/2002                                                 | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#ifndef _AVT_TEST_TOOL_H_
#define _AVT_TEST_TOOL_H_

//#include <unistd.h>
//#include <stdio.h>
//#include <string.h>
#include <pal_printf.h>
#include <pal_util.h>

#include <a653.h>
#include <imaspex.h>
#include "auxlib.h"
#include "AVT_TARGET_ImplementationDependent.h"
#include "AVT_TestsID.h"

#define NO_LOG_ERROR 0
#define LOG_ERROR    1

#define AVT_NO_ACTIVE 0
#define AVT_ACTIVE    1 

#define START_PROCESS 0
#define STOP_PROCESS  1

#define AVT_ERROR 1


typedef enum
{
  NO_AUX_PROCESS = 0,          

  P1_T_API_PART_230_0020 = 1,
  P1_T_API_PART_230_0030 = 2,
  P1_T_API_PART_230_0035 = 3,     

  P1_T_API_PROC_240_0010 = 4,
  P1_T_API_PROC_240_0013 = 5,
  P1_T_API_PROC_240_0016 = 6,
  P1_T_API_PROC_240_0020 = 7,
  P1_T_API_PROC_240_0023 = 8,
  P1_T_API_PROC_240_0026 = 9,
  P1_T_API_PROC_240_0030 = 10,
  P1_T_API_PROC_240_0040 = 11,
  P1_T_API_PROC_250_0010 = 12,
  P1_T_API_PROC_250_0020 = 13,
  P1_T_API_PROC_250_0030 = 14,
  P1_T_API_PROC_250_0040 = 15,
  P1_T_API_PROC_250_0050 = 16,
  P1_T_API_PROC_260_0010 = 18,
  P1_T_API_PROC_260_0015 = 19,
  P1_T_API_PROC_260_0020 = 178,
  P1_T_API_PROC_270_0010 = 20,
  P1_T_API_PROC_270_0020 = 21,
  P1_T_API_PROC_270_0040 = 22,
  P1_T_API_PROC_270_0050 = 23,
  P1_T_API_PROC_270_0060 = 24,
  P1_T_API_PROC_270_0065 = 25,
  P1_T_API_PROC_270_0070 = 26,
  P1_T_API_PROC_280_0010 = 27,
  P1_T_API_PROC_280_0020 = 28,
  P1_T_API_PROC_280_0030 = 29,
  P1_T_API_PROC_280_0040 = 30,     
  P1_T_API_PROC_290_0010 = 31,
  P1_T_API_PROC_290_0020 = 32,
  P1_T_API_PROC_290_0030 = 33,
  P1_T_API_PROC_290_0040 = 34,
  P1_T_API_PROC_300_0010 = 35,
  P1_T_API_PROC_300_0020 = 36,
  P1_T_API_PROC_310_0010 = 39,     
  P1_T_API_PROC_310_0020 = 40,     
  P1_T_API_PROC_310_0030 = 41,     
  P1_T_API_PROC_310_0040 = 42,     
  P1_T_API_PROC_310_0050 = 43,     
  P1_T_API_PROC_310_0060 = 44,     
  P1_T_API_PROC_320_0011 = 47,
  P1_T_API_PROC_320_0013 = 48,     
  P1_T_API_PROC_330_0011 = 49,
  P1_T_API_PROC_330_0013 = 50,     

  P1_T_API_TIME_030_0011 = 51,
  P1_T_API_TIME_030_0012 = 52,
  P1_T_API_TIME_030_0013 = 53,
  P1_T_API_TIME_040_0012 = 55,
  P1_T_API_TIME_040_0013 = 56,    
  P1_T_API_TIME_040_0014 = 57,
  P1_T_API_TIME_060_0050 = 59,    

  P1_T_API_INTER_580_0028 = 60,    
  P1_T_API_INTER_580_0033 = 61,    
  P1_T_API_INTER_580_0035 = 62,
  P1_T_API_INTER_580_0038 = 63,
  P1_T_API_INTER_580_0039 = 64,
  P1_T_API_INTER_580_0040 = 65,
  P1_T_API_INTER_580_0042 = 66,    
  P1_T_API_INTER_580_0045 = 67,    
  P1_T_API_INTER_580_0050 = 68,
  P1_T_API_INTER_580_0052 = 69,
  P1_T_API_INTER_580_0055 = 70,
  P1_T_API_INTER_580_0060 = 71,    
  P1_T_API_INTER_580_0062 = 72,    
  P1_T_API_INTER_580_0065 = 73,
  P1_T_API_INTER_580_0080 = 74,    
  P1_T_API_INTER_580_0090 = 75,    
  P1_T_API_INTER_590_0045 = 76,
  P1_T_API_INTER_590_0050 = 77,     
  P1_T_API_INTER_590_0060 = 78,     
  P1_T_API_INTER_590_0066 = 79,     
  P1_T_API_INTER_590_0080 = 80,   
  P1_T_API_INTER_590_0090 = 81,   

  P1_T_API_INTRA_250_0010 = 82,
  P1_T_API_INTRA_250_0020 = 83,
  P1_T_API_INTRA_250_0030 = 84,
  P1_T_API_INTRA_250_0040 = 85,
  P1_T_API_INTRA_250_0050 = 86,
  P1_T_API_INTRA_250_0075 = 87,
  P1_T_API_INTRA_250_0080 = 88,
  P1_T_API_INTRA_250_0090 = 89,
  P1_T_API_INTRA_250_0100 = 90,
  P1_T_API_INTRA_250_0120 = 91,    
  P1_T_API_INTRA_250_0130 = 92,    
  P1_T_API_INTRA_260_0010 = 93,
  P1_T_API_INTRA_260_0020 = 94,
  P1_T_API_INTRA_260_0030 = 95,
  P1_T_API_INTRA_260_0040 = 96,
  P1_T_API_INTRA_260_0065 = 97,
  P1_T_API_INTRA_260_0070 = 98,
  P1_T_API_INTRA_260_0080 = 99,
  P1_T_API_INTRA_260_0090 = 100,
  P1_T_API_INTRA_260_0110 = 101,    
  P1_T_API_INTRA_260_0120 = 102,    
  P1_T_API_INTRA_300_0010 = 103,
  P1_T_API_INTRA_300_0020 = 104,
  P1_T_API_INTRA_310_0010 = 105,
  P1_T_API_INTRA_310_0035 = 106,
  P1_T_API_INTRA_310_0040 = 107,
  P1_T_API_INTRA_310_0050 = 108,
  P1_T_API_INTRA_310_0060 = 109,
  P1_T_API_INTRA_360_0010 = 110,
  P1_T_API_INTRA_360_0030 = 111,
  P1_T_API_INTRA_360_0035 = 112,
  P1_T_API_INTRA_360_0040 = 113,
  P1_T_API_INTRA_360_0050 = 114,
  P1_T_API_INTRA_360_0060 = 115,
  P1_T_API_INTRA_360_0080 = 116,
  P1_T_API_INTRA_360_0090 = 117,
  P1_T_API_INTRA_370_0020 = 118,
  P1_T_API_INTRA_370_0030 = 119,
  P1_T_API_INTRA_370_0040 = 120,
  P1_T_API_INTRA_370_0050 = 121,
  P1_T_API_INTRA_410_0010 = 122,
  P1_T_API_INTRA_410_0020 = 123,
  P1_T_API_INTRA_410_0030 = 124,
  P1_T_API_INTRA_430_0035 = 125,
  P1_T_API_INTRA_410_0040 = 126,
  P1_T_API_INTRA_430_0010 = 127,
  P1_T_API_INTRA_430_0040 = 128,
  P1_T_API_INTRA_430_0050 = 129,
  P1_T_API_INTRA_430_0060 = 130,

  P1_T_API_HMON_080_0011 = 131,     
  P1_T_API_HMON_080_0012 = 132,     
  P1_T_API_HMON_100_0011 = 133,
  P1_T_API_HMON_100_0020 = 134,     
  P1_T_API_HMON_110_0011 = 135,
  P1_T_API_HMON_110_0012 = 136,
  P1_T_API_HMON_110_0015 = 137,
  P1_T_API_HMON_120_0010 = 138,     
  P1_T_API_HMON_120_0020 = 139,     
  P1_T_API_HMON_120_0030 = 140,     
  P1_T_API_HMON_120_0040 = 141,     
  
  
  /*-----------------------------------*/
  
  
  P2_T_API_PROC_240_0010 = 142,
  P2_T_API_PROC_240_0013 = 143,
  P2_T_API_PROC_240_0016 = 144,
  P2_T_API_PROC_240_0020 = 145,
  P2_T_API_PROC_240_0023 = 146,
  P2_T_API_PROC_240_0026 = 147,
  P2_T_API_PROC_240_0030 = 148,
  P2_T_API_PROC_240_0040 = 149,
  P2_T_API_PROC_260_0010 = 150,
  P2_T_API_PROC_260_0015 = 151,
  P2_T_API_PROC_270_0040 = 152,
  P2_T_API_PROC_270_0050 = 153,
  P2_T_API_PROC_280_0020 = 154,
  P2_T_API_PROC_280_0030 = 155,
  P2_T_API_PROC_280_0040 = 156,    
  P2_T_API_PROC_290_0010 = 157,
  P2_T_API_PROC_290_0020 = 158,
  P2_T_API_PROC_290_0030 = 159,
  P2_T_API_PROC_290_0040 = 160,
  
  P2_T_API_TIME_040_0011 = 161,
  
  P2_T_API_INTER_580_0039 = 162,    
  P2_T_API_INTER_580_0045 = 163,    
  P2_T_API_INTER_580_0055 = 164,    
  P2_T_API_INTER_580_0065 = 165,    
  P2_T_API_INTER_580_0080 = 166,    
  P2_T_API_INTER_580_0090 = 167,    
  P2_T_API_INTER_590_0080 = 168,    
  P2_T_API_INTER_590_0090 = 169,    
  
  P2_T_API_INTRA_250_0120 = 170,    
  P2_T_API_INTRA_250_0130 = 171,    
  P2_T_API_INTRA_260_0110 = 172,    
  P2_T_API_INTRA_260_0120 = 173,    
  P2_T_API_INTRA_360_0080 = 174,
  P2_T_API_INTRA_360_0090 = 175,

  
  /*-----------------------------------*/


  P100_T_API_PROC_250_0055 = 17,
  P100_T_API_PROC_260_0020 = 176,
  P100_T_API_PROC_300_0050 = 37, 
  P100_T_API_PROC_300_0060 = 38,       
  P100_T_API_PROC_310_0070 = 45,     
  P100_T_API_PROC_310_0080 = 46,
  
  P100_T_API_TIME_040_0011 = 54,
  P100_T_API_TIME_060_0020 = 58    
  


} SubTestsNumber;

#include "AVT_Wrapper.h"
#include "AVT_Funcs.h"

#include "AVT_Tools.h"

#endif
/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                   END OF FILE: AVT_TestTool                        | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
