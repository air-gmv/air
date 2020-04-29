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
 * | FILE: Test_INTER_560_0011.c                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): MasterProcedure_T_API_INTER_560_0011        | *
 * |                                                                    | *
 * | DESCRIPTION: SUCCESSFUL SAMPLING PORT STATUS CHECK.                | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    09/10/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0560:0010, API-INTER-0560                   | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_560_0011.h"


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_INTER_560_0012                | *
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
MasterProcedure_T_API_INTER_560_0011( TEST_RESULT_TYPE *TestResult )
{
  BOOLEAN                        FlagOK      = TRUE;
  RETURN_CODE_TYPE               RetCode     = NO_ERROR;
  SAMPLING_PORT_ID_TYPE          SamplingPortId = 0;
  SAMPLING_PORT_STATUS_TYPE      PortStatus;
  GET_SAMPLING_PORT_STATUS_ERROR StatusError;
printf("MasterProcedure_T_API_INTER_560_0011\n");
  PortStatus.MAX_MESSAGE_SIZE = SAMPLING_PORT_MAX_MESSAGE_SIZE;
  PortStatus.PORT_DIRECTION = DESTINATION;
  PortStatus.REFRESH_PERIOD = SAMPLING_PORT_REFRESH_TIME;
  PortStatus.LAST_MSG_VALIDITY = INVALID;
  
  /* Get port id */
  w_GET_SAMPLING_PORT_ID ( "SP_MASTER_RECEIVE", &SamplingPortId, &RetCode );
  EvaluateError( RetCode, &FlagOK );

  /* wait to assure sampling port message is invalid */

  printf("waiting..\n");
   rtems_task_wake_after(SAMPLING_PORT_REFRESH_TICKS +1);


/*  w_TIMED_WAIT( SAMPLING_PORT_REFRESH_TIME + 1, &RetCode );
  EvaluateError( RetCode, &FlagOK );*/

  /* Get the port status */
  StatusError = CheckGetSamplingPortStatus( SamplingPortId,
					    NO_ERROR,
					    &PortStatus,
					    &RetCode );
  
  /* Evaluate errors */
  EvaluateError( RetCode, &FlagOK );
  if( StatusError.ReturnCode != NO_ERROR )
  {
    FlagOK = FALSE;
  }
  
  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );

} /* Test_T_API_INTER_560_0011 */



/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_INTER_560_0011.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
