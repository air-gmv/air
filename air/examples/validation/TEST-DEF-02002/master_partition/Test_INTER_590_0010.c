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
 * | FILE: Test_INTER_590_0010.c                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): MasterProcedure_T_API_INTER_590_0010        | *
 * |                                                                    | *
 * | DESCRIPTION: TEST THE SERVICE RECEIVE_QUEUING_MESSAGE WHEN THE FIFO| *
 * | QUEUE IS NOT EMPTY AND LENGTH OF MESSAGE IS <= MAXIMUM_LENGTH      | *
 * | ( INPUT PARAMETER OF THE SERVICE ).                                | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    21/10/2002                                                | *
 * |                                                                    | *
 * | CVRS COVERED: API-INTER-590:0010, API-INTER-590                    | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_590_0010.h"


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_INTER_590_0010                | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   21/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCESS or FAILED | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
MasterProcedure_T_API_INTER_590_0010( TEST_RESULT_TYPE *TestResult )
{
  RETURN_CODE_TYPE               RetCode = NO_ERROR;
  BOOLEAN                        FlagOK  = TRUE;
  BOOLEAN                        AuxPartitionStatus  = TRUE;
  QUEUING_PORT_ID_TYPE           PortId  = 0;
  APEX_BYTE                      Message[QUEUING_PORT_MAX_MESSAGE_SIZE];
  MESSAGE_SIZE_TYPE              Length;
  QUEUING_PORT_STATUS_TYPE       PortStatus;
  RECEIVE_QUEUING_MESSAGE_ERROR  PortError;
printf("MasterProcedure_T_API_INTER_590_0010\n");
  /* Send subtestid to AuxPartition */
  RetCode = SendSubtestIdToAuxPartition( T_API_INTER_590_0010 );
  EvaluateError( RetCode, &FlagOK );

  /* wait to assure the other partition has execute task*/
  /* Read AuxPartition results */
  RetCode = ReadResultsFromAuxPartition( &AuxPartitionStatus );
  EvaluateError( RetCode, &FlagOK );
  EvaluateVariable ( AuxPartitionStatus, TRUE, &FlagOK );

 /* Get "QP_MASTER_SEND" port id */
  w_GET_QUEUING_PORT_ID ( "QP_MASTER_RECEIVE", &PortId, &RetCode );
  EvaluateError( RetCode, &FlagOK );

  /* Clean received area */
  memset( Message, '\0', sizeof( Message ));

  /* Try read message from queuing port */
  PortError = CheckReceiveQueuingMessage( PortId,
                                          0,     /* timeout */
                                          "Hello world",
                                          11,
                                          NO_ERROR,
                                          Message,
                                          &Length,
                                          &PortStatus,
                                          &RetCode );

  /* Evaluate errors */
  EvaluateError( RetCode, &FlagOK  );

  if( PortError.ReturnCode   != NO_ERROR ||
      PortError.MessageSize  != NO_ERROR ||
      PortError.Message      != NO_ERROR ||
      PortError.GetStatus    != NO_ERROR )
  {
    FlagOK = FALSE;
  }

  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );
} /* MasterProcedure_T_API_INTER_590_0010 */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_INTER_590_0010.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
