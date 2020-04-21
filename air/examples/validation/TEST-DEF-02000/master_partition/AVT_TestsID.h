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
  T_API_PART_220_0010 = 0,
  T_API_PART_230_0010 = 1,
  T_API_PART_230_0020 = 2,
  T_API_PART_230_0030 = 3,
  T_API_PART_230_0035 = 4,   
  T_API_PART_230_0040 = 5,
  T_API_PART_230_0050 = 6,  
  T_API_PART_230_0060 = 7,  

  
  
  /* signals last test. Also used to aux partition to terminate */ 
  LAST_TEST = 8

} TestsNumber;


#endif
/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                   END OF FILE: AVT_TestsID.h                       | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */

