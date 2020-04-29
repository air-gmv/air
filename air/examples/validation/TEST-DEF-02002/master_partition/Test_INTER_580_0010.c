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
 * | FILE: Test_INTER_580_0010.c                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): MasterProcedure_T_API_INTER_580_0010        | *
 * |                                                                    | *
 * | DESCRIPTION: TEST THE SERVICE SEND_QUEING_MESSAGE WITH ENOUGH SPACE| *
 * | AND NO PROCESSES WAITING                                           | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    21/10/2002                                                | *
 * |                                                                    | *
 * | CVRS COVERED: API- INTER -580:0010, API- INTER -580                | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_580_0010.h"


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_INTER_580_0010                | *
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
MasterProcedure_T_API_INTER_580_0010( TEST_RESULT_TYPE *TestResult )
{
  RETURN_CODE_TYPE            RetCode = NO_ERROR;
  BOOLEAN                     FlagOK  = TRUE;
  BOOLEAN                     AuxPartitionStatus  = TRUE;
  QUEUING_PORT_ID_TYPE        PortId  = 0;
  APEX_BYTE                   Message[QUEUING_PORT_MAX_MESSAGE_SIZE];
  QUEUING_PORT_STATUS_TYPE    PortStatus;
  SEND_QUEUING_MESSAGE_ERROR  PortError;

  printf("MasterProcedure_T_API_INTER_580_0010\n");

  /* Send subtestid to AuxPartition */
  RetCode = SendSubtestIdToAuxPartition( T_API_INTER_580_0010 );
  EvaluateError( RetCode, &FlagOK );


  /* Enable preemption */
/*  RetCode = UnlockPreemption();
  EvaluateError( RetCode, &FlagOK );*/

  /* Compose message to send */
  memset(Message, '\0', sizeof(Message));
  strcpy( Message, "Message sent" );

 /* Get "QP_MASTER_SEND" port id */
  w_GET_QUEUING_PORT_ID ( "QP_MASTER_SEND", &PortId, &RetCode );
  EvaluateError( RetCode, &FlagOK );

  printf("Message: %s\n", Message);

  /* Call CheckSendQueuingMessage */
  PortError = CheckSendQueuingMessage( PortId,
                                       Message,
                                       strlen(Message),
                                       0,
                                       NO_ERROR,
                                       &PortStatus,
                                       &RetCode );

  /* Evaluate errors */
  EvaluateError( RetCode, &FlagOK );
  if( PortError.ReturnCode != NO_ERROR ||
      PortError.GetStatus  != NO_ERROR )
  {
    FlagOK = FALSE;
  }

  /* Read AuxPartition results */
  printf("MasterProcedure_T_API_INTER_580_0010 calling ReadResultsFromAuxPartition\n");
  RetCode = ReadResultsFromAuxPartition( &AuxPartitionStatus );
  EvaluateError( RetCode, &FlagOK );

  EvaluateVariable ( AuxPartitionStatus, TRUE, &FlagOK );
  
  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );

} /* MasterProcedure_T_API_INTER_580_0010 */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_INTER_580_0010.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
