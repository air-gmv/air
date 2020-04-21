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
 * | FILE: MasterProcedure_T_API_PART_230_0035                          | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_PART                                           | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): MasterProcedure_T_API_PART_230_0035         | *
 * |                                                                    | *
 * | DESCRIPTION: TEST THE SERVICE SET_PARTITION_MODE TO COLD_START.    | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    15/09/2004                                                | *
 * |                                                                    | *
 * | CVRS COVERED: API-PART-230:0035                                    | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#include "Test_PART_230_0035.h"

extern int TraceActive;

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_PART_230_0035                 | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   15/09/2004                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCESS or FAILED | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
MasterProcedure_T_API_PART_230_0035( TEST_RESULT_TYPE *TestResult )
{  
  RETURN_CODE_TYPE    RetCode = NO_ERROR;
  BOOLEAN             FlagOK = TRUE;
  APEX_BYTE           Message[QUEUING_PORT_MAX_STARTUPINFO_SIZE];
  StartUpInfoType     StartUpInfo;

  printf("MasterProcedure_T_API_PART_230_0035\n");

  /* Send subtestid to AuxPartition making it to go to COLD_START */
  RetCode = SendSubtestIdToAuxPartition( T_API_PART_230_0035 );
  EvaluateError( RetCode, &FlagOK );
  printf("HERE\n\n\n\n\n\n");
  memset ( Message, '\0', sizeof ( StartUpInfoType ) );
  RetCode = ReceiveQueuingMessage ( "QP_AUXPARTITONSTARTUP_RECEIVE", 
                                    INFINITE_TIME_VALUE, 
                                    Message );
  EvaluateError( RetCode, &FlagOK );



  if ( FlagOK == TRUE )
  {
     memcpy ( &StartUpInfo, Message, sizeof ( StartUpInfoType ) );
     if ( ( StartUpInfo.OperatingMode != COLD_START ) ||
          ( StartUpInfo.StartCondition != PARTITION_RESTART) )
	  {
		  FlagOK = FALSE;
	  }
  }
  
  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );
} /* MasterProcedure_T_API_PART_230_0035 */

/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_PART_230_0035.c                    | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
