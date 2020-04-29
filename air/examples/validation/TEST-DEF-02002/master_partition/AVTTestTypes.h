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
 * | FILE: AVTTestTypes.h                                               | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVTTESTTYPES                                            | *
 * | UNIT NAME: AVT                                                     | *
 * |                                                                    | *
 * | DESCRIPTION: Header file with functions prototypes, definitions    | *
 * |              concern to AVTTestTypes.c source.                     | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   10/11/2002                                                 | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#ifndef _AVT_TEST_TYPES_H_
#define _AVT_TEST_TYPES_H_

#include "AVT_TestTool.h"

BOOLEAN Test_GEN_010( APEX_BYTE );
BOOLEAN Test_GEN_020( APEX_INTEGER );
BOOLEAN Test_GEN_030( APEX_UNSIGNED );
BOOLEAN Test_GEN_040( APEX_LONG_INTEGER );
BOOLEAN Test_GEN_050( NAME_TYPE );
BOOLEAN Test_GEN_060( RETURN_CODE_TYPE );
BOOLEAN Test_GEN_070( SYSTEM_ADDRESS_TYPE );
BOOLEAN Test_GEN_080( SYSTEM_TIME_TYPE );
BOOLEAN Test_GEN_090( MESSAGE_ADDR_TYPE );
BOOLEAN Test_GEN_100( MESSAGE_SIZE_TYPE );

BOOLEAN  Test_PART_010( OPERATING_MODE_TYPE );
BOOLEAN  Test_PART_020( PARTITION_ID_TYPE );
BOOLEAN  Test_PART_030( LOCK_LEVEL_TYPE );
BOOLEAN  Test_PART_040( START_CONDITION_TYPE );
BOOLEAN  Test_PART_050( void ); /*to call on general*/

BOOLEAN  Test_PROC_010( PROCESS_ID_TYPE );
BOOLEAN  Test_PROC_020( PROCESS_NAME_TYPE );
BOOLEAN  Test_PROC_030( PRIORITY_TYPE );
BOOLEAN  Test_PROC_040( STACK_SIZE_TYPE );
BOOLEAN  Test_PROC_050( LOCK_LEVEL_TYPE );
/*BOOLEAN  Test_PROC_070( PROCESS_STATE_TYPE );
BOOLEAN  Test_PROC_080( DEADLINE_TYPE );
BOOLEAN  Test_PROC_090( PROCESS_ATTRIBUTES_TYPE );*/
BOOLEAN  Test_PROC_100( );/*to call on general function*/

BOOLEAN Test_INTER_010( PORT_DIRECTION_TYPE );
BOOLEAN Test_INTER_020( void );/*to call on general function*/
BOOLEAN Test_INTER_030( void );/*to call on general function*/
BOOLEAN Test_INTER_040( VALIDITY_TYPE );
BOOLEAN Test_INTER_050( void );/*to call on general function*/
BOOLEAN Test_INTER_060( void );/*to call on general function*/
BOOLEAN Test_INTER_070( void );/*to call on general function*/
BOOLEAN Test_INTER_080( void );/*to call on general function*/
BOOLEAN Test_INTER_090( MESSAGE_RANGE_TYPE );
BOOLEAN Test_INTER_100( WAITING_RANGE_TYPE );
BOOLEAN Test_INTER_110( void );/*to call on general function*/

BOOLEAN Test_INTRA_010 ( void );/*to call on general function*/
BOOLEAN Test_INTRA_020 ( void );/*to call on general function*/
BOOLEAN Test_INTRA_030 ( void );/*to call on general function*/
BOOLEAN Test_INTRA_040 ( void );/*to call on general function*/
BOOLEAN Test_INTRA_050 ( void );/*to call on general function*/
/*BOOLEAN Test_INTRA_060 ( EMPTY_INDICATOR_TYPE );*/
BOOLEAN Test_INTRA_070 ( void );/*to call on general function*/
BOOLEAN Test_INTRA_080 ( void );/*to call on general function*/
BOOLEAN Test_INTRA_090 ( void );/*to call on general function*/
/*BOOLEAN Test_INTRA_100 ( SEMAPHORE_VALUE_TYPE );*/
BOOLEAN Test_INTRA_110 ( void );/*to call on general function*/
BOOLEAN Test_INTRA_120 ( void );/*to call on general function*/
BOOLEAN Test_INTRA_130 ( void );/*to call on general function*/
/*BOOLEAN Test_INTRA_140 ( EVENT_STATE_TYPE );*/
BOOLEAN Test_INTRA_150 ( void );/*to call on general function*/

BOOLEAN Test_HMON_010 ( ERROR_MESSAGE_TYPE );
BOOLEAN Test_HMON_020 ( ERROR_MESSAGE_SIZE_TYPE );
BOOLEAN Test_HMON_030 ( ERROR_CODE_TYPE );
BOOLEAN Test_HMON_040 ( void ); /*to call on general function*/

BOOLEAN General_API_Test ( void );

#endif

/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                   END OF FILE: AVTTestTypes.h                      | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
