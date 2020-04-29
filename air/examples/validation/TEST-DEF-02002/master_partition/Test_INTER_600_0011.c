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
 * | FILE: Test_INTER_600_0011.c                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): MasterProcedure_T_API_INTER_600_0011        | *
 * |                                                                    | *
 * | DESCRIPTION: SUCCESSFUL QUEUING PORT ID CHECK.                     | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    08/10/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0600:0010, API-INTER-0600                   | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_600_0011.h"

QUEUING_PORT_ID_TYPE QueuingPortIdReference = 0;

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_INTER_600_0011                | *
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
MasterProcedure_T_API_INTER_600_0011( TEST_RESULT_TYPE *TestResult  )
{
  BOOLEAN                    FlagOK  = TRUE;
  RETURN_CODE_TYPE           RetCode       = NO_ERROR;
  QUEUING_PORT_ID_TYPE       QueuingPortId = 0;
  GET_QUEUING_PORT_ID_ERROR  PortError;
  QUEUING_PORT_STATUS_TYPE   PortStatus;

  printf("MasterProcedure_T_API_INTER_600_0011\n");

  /* Get queuing port id */
  PortError = CheckGetQueuingPortId( "QP_MASTER_SEND",
				                         NO_ERROR,
				                         &QueuingPortId,
				                         &PortStatus,
				                         &RetCode );
  
  /* Evaluate errors */
  EvaluateError( RetCode, &FlagOK );
  if(PortError.ReturnCode != NO_ERROR ||
     PortError.GetStatus  != NO_ERROR )
  {
    FlagOK = TRUE;
  }

  if ( QueuingPortId != QueuingPortIdReference )
  {
	  FlagOK = FALSE;
  }
  
  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );
  
} /* Test_T_API_INTER_600_0011 */

/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_INTER_600_0011.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
