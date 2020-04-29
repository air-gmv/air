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
 * | FILE: AVT_TestsID.h                                                | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVT_TESTID                                              | *
 * | UNIT NAME: AVT                                                     | *
 * |                                                                    | *
 * | DESCRIPTION: Header file with definitions used by all the modules. | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   08/09/2005                                                 | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#ifndef _AVT_TESTSID_H_
#define _AVT_TESTSID_H_

typedef enum 
{
	NOT_PERFORMED = 0,
	FAILED = 1,
	SUCESS = 2
}
TEST_RESULT_TYPE;


typedef enum
{
  

  T_API_PART_220_0010  = 0,
  T_API_INTER_560_0011 = 1,
  T_API_INTER_560_0012 = 2,
  T_API_INTER_570_0011 = 3,
  T_API_INTER_570_0012 = 4,
  T_API_INTER_570_0013 = 5,
  T_API_INTER_580_0010 = 6,
  T_API_INTER_580_0025 = 7,    
  T_API_INTER_580_0028 = 8,    
  T_API_INTER_580_0030 = 9,
  T_API_INTER_580_0032 = 10,    
  T_API_INTER_580_0033 = 11,    
  T_API_INTER_580_0035 = 12,
  T_API_INTER_580_0038 = 13,    
  T_API_INTER_580_0039 = 14,    
  T_API_INTER_580_0040 = 15,
  T_API_INTER_580_0042 = 16,    
  T_API_INTER_580_0045 = 17,    
  T_API_INTER_580_0050 = 18,
  T_API_INTER_580_0052 = 19,    
  T_API_INTER_580_0055 = 20,    
  T_API_INTER_580_0060 = 21,
  T_API_INTER_580_0062 = 22,    
  T_API_INTER_580_0065 = 23,    
  T_API_INTER_580_0070 = 24,
  T_API_INTER_580_0080 = 25,    
  T_API_INTER_580_0090 = 26,    
  T_API_INTER_590_0010 = 27,
  T_API_INTER_590_0020 = 28,
  T_API_INTER_590_0030 = 29,
  T_API_INTER_590_0040 = 30,
  T_API_INTER_590_0045 = 31,
  T_API_INTER_590_0050 = 32,
  T_API_INTER_590_0060 = 33,
  T_API_INTER_590_0066 = 34,    
  T_API_INTER_590_0070 = 35,
  T_API_INTER_590_0080 = 36,    
  T_API_INTER_590_0090 = 37,    
  T_API_INTER_600_0011 = 38,
  T_API_INTER_600_0012 = 39,
  T_API_INTER_610_0011 = 40,
  T_API_INTER_610_0012 = 41,
  
  
  
  /* signals last test. Also used to aux partition to terminate */ 
  LAST_TEST = 42

} TestsNumber;


#endif
/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                   END OF FILE: AVT_TestsID.h                       | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */

