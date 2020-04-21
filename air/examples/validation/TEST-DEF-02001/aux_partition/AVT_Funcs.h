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
 * | FILE: AVT_Funcs.h                                                  | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVTFUNCS                                                | *
 * | UNIT NAME: AVT                                                     | *
 * |                                                                    | *
 * | DESCRIPTION: Header file with functions prototypes, definitions    | *
 * |              concern to AVTFuncs.c source.                         | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#ifndef _AVT_FUNCS_H_
#define _AVT_FUNCS_H_

#include "AVT_TestTool.h"
#include "AVT_LoadTables.h"
#include <string.h>

typedef struct
{
  OPERATING_MODE_TYPE    OperatingMode;
  START_CONDITION_TYPE	 StartCondition;
} StartUpInfoType;

RETURN_CODE_TYPE WriteSamplingMessage( SAMPLING_PORT_NAME_TYPE  SPName,
				       MESSAGE_ADDR_TYPE        Message,
				       MESSAGE_SIZE_TYPE        Length );

RETURN_CODE_TYPE ReadSamplingMessage( SAMPLING_PORT_NAME_TYPE  SPName,
				      MESSAGE_ADDR_TYPE        Message,
				      VALIDITY_TYPE            *Validity );

RETURN_CODE_TYPE SendQueuingMessage ( QUEUING_PORT_NAME_TYPE  QPName,
				      MESSAGE_ADDR_TYPE       Message,
				      MESSAGE_SIZE_TYPE       Length,
				      SYSTEM_TIME_TYPE        TimeOut,
				      int                     LogFlag);

RETURN_CODE_TYPE ReceiveQueuingMessage ( QUEUING_PORT_NAME_TYPE  QPName,
					 SYSTEM_TIME_TYPE        TimeOut,
					 MESSAGE_ADDR_TYPE       Message );

RETURN_CODE_TYPE CleanFullQueuingPort ( QUEUING_PORT_NAME_TYPE QPName );
RETURN_CODE_TYPE CleanQueuingPort ( QUEUING_PORT_NAME_TYPE QPName );

RETURN_CODE_TYPE SendSubtestIdToAuxPartition( int SubtestId );
RETURN_CODE_TYPE SendExecSignalToMasterPartition ( void );
RETURN_CODE_TYPE ReadResultsFromAuxPartition ( BOOLEAN *Result );
RETURN_CODE_TYPE WriteResultsFromAuxPartition( BOOLEAN Result );
/*RETURN_CODE_TYPE ClearBlackboard( BLACKBOARD_NAME_TYPE BBName );*/

RETURN_CODE_TYPE UnlockPreemption( void );
RETURN_CODE_TYPE LockPreemption( int NumLevel, LOCK_LEVEL_TYPE *Level );


TEST_RESULT_TYPE    EvaluateResult( BOOLEAN FlagOK );
void                EvaluateProcessState(int State1, int State2, BOOLEAN *FlagOK);
void                EvaluateMessage( char *str1, char *str2, BOOLEAN *Result );
void                PutOkFailInMessage( BOOLEAN FlagOK, char * Message );
void  EvaluateError( RETURN_CODE_TYPE RetCode, BOOLEAN *FlagOK  );

void  EvaluateErrorType( RETURN_CODE_TYPE RetCode,
			 BOOLEAN *FlagOK,
			 RETURN_CODE_TYPE Error );

void  EvaluateVariable( BOOLEAN GV1, BOOLEAN value, BOOLEAN *FlagOK );
void  EvaluateNumValue( int value1, int value2, BOOLEAN *FlagOK ) ;


/***** error log  ***********/


void
WriteLogReturnCode( char             *Function, 
		    RETURN_CODE_TYPE  ExpError, 
		    RETURN_CODE_TYPE  ObtError );



void GetErrorName( RETURN_CODE_TYPE RetCode, char * ErrorName );

void WriteLogErrorFunc(  RETURN_CODE_TYPE RetCode,  
			 char             *FunctionName,
			 char             *Action );

void WriteLogErrorNum( char *FunctionName,
		       char *Param,
		       long ExpectedValue,
		       long ObtainedValue);

void WriteLogErrorStr(  char      *FunctionName,
			char      *Param,
			MESSAGE_ADDR_TYPE ExpectedMsg,
			MESSAGE_ADDR_TYPE ObtainedMsg );

void EvaluatePriority( long Priority1, long Priority2, BOOLEAN *FlagOK );

#endif

/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                       END OF FILE: AVT_Funcs.h                     | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
