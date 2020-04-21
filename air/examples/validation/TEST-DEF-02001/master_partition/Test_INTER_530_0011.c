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
 * | FILE: Test_INTER_530_0011.c                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): MasterProcedure_T_API_INTER_530_0011        | *
 * |                                                                    | *
 * | DESCRIPTION:  SUCCESSFUL SAMPLING MESSAGE WRITE.                   | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    18/10/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0530:010, API-INTER-0530                    | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_530_0011.h"


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_INTER_530_0011                | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   18/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCESS or FAILED | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
MasterProcedure_T_API_INTER_530_0011( TEST_RESULT_TYPE *TestResult )
{
  RETURN_CODE_TYPE             RetCode = NO_ERROR;
  BOOLEAN                      FlagOK  = TRUE;
  BOOLEAN                      AuxPartitionStatus  = TRUE;
  SAMPLING_PORT_ID_TYPE        PortId  = 0;
  APEX_BYTE                    Message[SAMPLING_PORT_MAX_MESSAGE_SIZE];
  SAMPLING_PORT_STATUS_TYPE    PortStatus;
  WRITE_SAMPLING_MESSAGE_ERROR WriteError;

printf("MasterProcedure_T_API_INTER_530_0011\n");
  /* Compose message to send for AuxPartition */
  memset (Message, 0, sizeof(Message)); 
  strcpy( Message, "sampling_message_test" );

  /* Execute CheckWriteSamplingMessage with a valid id */
  w_GET_SAMPLING_PORT_ID( "SP_MASTER_SEND", &PortId, &RetCode );
  EvaluateError( RetCode, &FlagOK );



  WriteError = CheckWriteSamplingMessage( PortId,
					  Message,
					  strlen( Message ),
					  NO_ERROR,
					  &PortStatus,
					  &RetCode );

  /* Evaluate errors */
  EvaluateError( RetCode, &FlagOK );
  if( WriteError.ReturnCode != NO_ERROR ||
      WriteError.GetStatus  != NO_ERROR )
  {
    FlagOK = FALSE;
  }

  /* Send subtestid to AuxPartition since sampling message was already written*/
  RetCode = SendSubtestIdToAuxPartition( T_API_INTER_530_0011 );
  EvaluateError( RetCode, &FlagOK );

  

  /* Read AuxPartition results */

  RetCode = ReadResultsFromAuxPartition( &AuxPartitionStatus );

  EvaluateError( RetCode, &FlagOK );
  EvaluateVariable ( AuxPartitionStatus, TRUE, &FlagOK );

   /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );

} /* MasterProcedure_T_API_INTER_530_0011  */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_INTER_530_0011.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
