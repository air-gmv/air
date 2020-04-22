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
 * | FILE: MasterProcedure_T_API_PART_230_0020                          | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_PART                                           | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): MasterProcedure_T_API_PART_230_0020         | *
 * |                                                                    | *
 * | DESCRIPTION: TEST THE SERVICE SET_PARTITION_MODE TO IDLE.          | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    04/10/2002                                                | *
 * |                                                                    | *
 * | CVRS COVERED: API-PART-230:0020                                    | * 
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#include "Test_PART_230_0020.h"

extern int TraceActive;

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_PART_230_0020                 | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   04/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCESS or FAILED | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
MasterProcedure_T_API_PART_230_0020( TEST_RESULT_TYPE *TestResult )
{  
  RETURN_CODE_TYPE    RetCode = NO_ERROR;
  BOOLEAN             FlagOK = TRUE;
  BOOLEAN             PartitionIdle = FALSE;
  APEX_BYTE           Message[QUEUING_PORT_MAX_STARTUPINFO_SIZE];
  
  printf("MasterProcedure_T_API_PART_230_0020\n");

  /* Send subtestid to AuxPartition making it to go IDLE */
  RetCode = SendSubtestIdToAuxPartition( T_API_PART_230_0020 );
  EvaluateError( RetCode, &FlagOK );
  
  memset ( Message, '0', sizeof ( StartUpInfoType ) );
  RetCode = ReceiveQueuingMessage ( "QP_AUXPARTITONSTARTUP_RECEIVE", 
                                    MAXIMUM_TIMEWAIT_FOR_RESTART, 
                                    Message );
  
  if ( RetCode != TIMED_OUT )
  {
     FlagOK = FALSE;
  }
	  
  /*IMPLEMENTATION DEPENDENT*/
  /*make aditional checks to see partition is idle*/
  if ( CheckThatPartitionIsIdle ( &PartitionIdle ) == DEPENDENCE_AVAILABLE )
  {
	  EvaluateVariable ( PartitionIdle, TRUE, &FlagOK );
  }
  /*END IMPLEMENTATION DEPENDENT*/

  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );
 printf("leaving test T_API_PART_230_0020\n");
} /* MasterProcedure_T_API_PART_230_0020 */

/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_PART_230_0020.c                    | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
