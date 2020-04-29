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
 * | FILE: Test_INTER_610_0011.c                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): MasterProcedure_T_API_INTER_610_0011        | *
 * |                                                                    | *
 * | DESCRIPTION:SUCCESSFUL QUEUING PORT STATUS CHECK.                  | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    09/10/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0610:0010, API-INTER-0610                   | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_610_0011.h"

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_INTER_610_0011                | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   09/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCESS or FAILED | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
MasterProcedure_T_API_INTER_610_0011( TEST_RESULT_TYPE *TestResult  )
{
  BOOLEAN                       FlagOK     = TRUE;
  RETURN_CODE_TYPE              RetCode    = NO_ERROR;
  QUEUING_PORT_ID_TYPE          QueuingPortId = 0;
  QUEUING_PORT_STATUS_TYPE      PortStatus;
  GET_QUEUING_PORT_STATUS_ERROR StatusError;

  printf("MasterProcedure_T_API_INTER_610_0011\n");

  PortStatus.NB_MESSAGE = QUEUING_PORT_MAX_MESSAGE_NB;
  PortStatus.MAX_MESSAGE_SIZE = QUEUING_PORT_MAX_MESSAGE_SIZE;
  PortStatus.PORT_DIRECTION = SOURCE;
  PortStatus.WAITING_PROCESSES = 0;
  
 /* Get "QP_MASTER_SEND" port id */
  w_GET_QUEUING_PORT_ID ( "QP_MASTER_SEND", &QueuingPortId, &RetCode );
  EvaluateError( RetCode, &FlagOK );

  /* Get the port status */
  StatusError = CheckQueuingPortStatus( QueuingPortId,
					NO_ERROR,
					&PortStatus,
					&RetCode );
  
  /* Evaliate errors */
  EvaluateError( RetCode, &FlagOK );
  if ( StatusError.ReturnCode != NO_ERROR )
  {
    FlagOK = FALSE;
  }
  
  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );
} /* Test_T_API_INTER_610_0011 */

/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_INTER_610_0011.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
