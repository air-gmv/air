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
 * | FILE: AVT_Tools.h                                                  | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVTTOOLS                                                | *
 * | UNIT NAME: AVT                                                     | *
 * |                                                                    | *
 * | DESCRIPTION: Header file with functions prototypes, definitions    | *
 * |              concern to AVTTools.c source.                         | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#ifndef _AVT_TOOLS_H_
#define _AVT_TOOLS_H_


#include "AVT_TestTool.h"

RETURN_CODE_TYPE DisplayQPStatus ( QUEUING_PORT_NAME_TYPE QPName );
RETURN_CODE_TYPE DisplayPartitionStatus( void );
RETURN_CODE_TYPE DisplayProcessStatus( PROCESS_NAME_TYPE PRCName );
RETURN_CODE_TYPE DisplayMyStatus( void );
void             DisplayStr( char *str, char *Message  );
void             DisplayNum( char *msg, RETURN_CODE_TYPE RetCode  );
void             DisplayError(  char *str, RETURN_CODE_TYPE RetCode );
/*RETURN_CODE_TYPE DisplayStatusSemaphore( SEMAPHORE_NAME_TYPE SMName );
RETURN_CODE_TYPE DisplayBufferStatus( BUFFER_NAME_TYPE BName );*/
RETURN_CODE_TYPE DisplayProcessState( PROCESS_NAME_TYPE PRCName );



void WriteLog(  char *str, RETURN_CODE_TYPE RetCode );

#endif

/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                      END OF FILE: AVT_Tools.h                      | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
