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
 * | FILE: Test_API_INTER_570_0011.c                                    | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): Test_T_API_INTER_570_0011                   | *
 * |                                                                    | *
 * | DESCRIPTION: QUEUING PORT CREATION.                                | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    09/10/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0570:0010, API-INTER-0570:0040,             | *
 * |              API-INTER-0570                                        | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_570_0011.h"


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: Test_T_API_INTER_570_0011                           | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   27/12/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCESS or FAILED | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
Test_T_API_INTER_570_0011( TEST_RESULT_TYPE *TestResult )
{
  BOOLEAN                     FlagOK     = TRUE;
  RETURN_CODE_TYPE            RetCode    = NO_ERROR;
  QUEUING_PORT_ID_TYPE        PortId     = 0;
  CREATE_QUEUING_PORT_ERROR   PortError;
  QUEUING_PORT_STATUS_TYPE    PortStatus;

  printf("Test_T_API_INTER_570_0011\n");

  PortError = CheckCreateQueuingPort( "QP_CREATE_TEST",
  			         QUEUING_PORT_MAX_MESSAGE_SIZE,
			         QUEUING_PORT_MAX_MESSAGE_NB,
				      SOURCE,
				      FIFO,
				      NO_ERROR,
				      &PortId,
				      &PortStatus,
				      &RetCode );

  /* Evaluate errors */
  EvaluateError( RetCode, &FlagOK );

  if( PortError.ReturnCode     != NO_ERROR ||
      PortError.GetStatus      != NO_ERROR ||
      PortError.MaxMessageSize != NO_ERROR ||
      PortError.MaxNBMessage   != NO_ERROR ||
      PortError.PortDirection  != NO_ERROR )
  {
     FlagOK = FALSE;
  }

  /* To try create the same port twice, a NO_ACTION */
  /* error should be raised */
  PortError =
    CheckCreateQueuingPort( "QP_CREATE_TEST",
	          QUEUING_PORT_MAX_MESSAGE_SIZE,
             QUEUING_PORT_MAX_MESSAGE_NB,
			    SOURCE,
			    FIFO,
			    NO_ACTION,
			    &PortId,
			    &PortStatus,
			    &RetCode );

  /* Evaluate errors */
  EvaluateErrorType( RetCode, &FlagOK, NO_ACTION );

  if( PortError.ReturnCode    != NO_ERROR ||
      PortError.GetStatus     != NO_ERROR ||
      PortError.MaxMessageSize != NO_ERROR ||
      PortError.MaxNBMessage   != NO_ERROR ||
      PortError.PortDirection != NO_ERROR )
  {
     FlagOK = FALSE;
  }

  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );

} /* Test_T_API_INTER_570_0011 */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |            END OF FILE: Test_T_API_INTER_570_0011.c                | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
