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
 * | FILE: Test_API_INTER_520_0011.c                                    | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): Test_T_API_INTER_520_0011                   | *
 * |                                                                    | *
 * | DESCRIPTION: SAMPLING PORT CREATION.                               | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    26/12/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-520_0011 ; API-INTER-015                    | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_520_0011.h"


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: Test_T_API_INTER_520_0011                           | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   26/12/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCESS or FAILED | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
Test_T_API_INTER_520_0011( TEST_RESULT_TYPE *TestResult )
{
  BOOLEAN                     FlagOK         = TRUE;
  RETURN_CODE_TYPE            RetCode        = NO_ERROR;
  SAMPLING_PORT_ID_TYPE       SamplingPortId = 0;
  CREATE_SAMPLING_PORT_ERROR  PortError;
  SAMPLING_PORT_STATUS_TYPE   PortStatus;

  printf("Test_T_API_INTER_520_0011\n");

  /* 1 - Create Sampling port "SP_MASTER_SEND" */
  PortError = CheckCreateSamplingPort( "SP_CREATE_TEST",
		             SAMPLING_PORT_MAX_MESSAGE_SIZE,
				       SOURCE,
				       SAMPLING_PORT_REFRESH_TIME,
				       NO_ERROR,
				       &SamplingPortId,
				       &PortStatus,
				       &RetCode );

  printf("calling second create\n");

  /* Evaluate errors */
  EvaluateError( RetCode, &FlagOK );

 if( PortError.ReturnCode     != NO_ERROR ||
      PortError.GetStatus      != NO_ERROR ||
      PortError.MaxMessageSize != NO_ERROR ||
      PortError.PortDirection  != NO_ERROR ||
      PortError.RefreshPeriod  != NO_ERROR )
  {
     FlagOK = FALSE;
  }



  /* Try create de same port twice, a NO_ACTION error */
  /* should be raised */
  PortError =  CheckCreateSamplingPort( "SP_CREATE_TEST",
					SAMPLING_PORT_MAX_MESSAGE_SIZE,
					SOURCE,
					SAMPLING_PORT_REFRESH_TIME,
					NO_ACTION,
					&SamplingPortId,
					&PortStatus,
					&RetCode );
  
  /* Evaluate errors */
  EvaluateErrorType( RetCode, &FlagOK, NO_ACTION );

  if( PortError.ReturnCode     != NO_ERROR ||
      PortError.GetStatus      != NO_ERROR ||
      PortError.MaxMessageSize != NO_ERROR ||
      PortError.PortDirection  != NO_ERROR ||
      PortError.RefreshPeriod  != NO_ERROR )
  {
     FlagOK = FALSE;
  }



  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );
  
} /* Test_T_API_INTER_520_0011 */



/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |             END OF FILE: Test_API_INTER_520_0011.c                 | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
