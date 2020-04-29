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
 * | FILE: Test_INTER_570_0013.c                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): Test_T_API_INTER_570_0013                   | *
 * |                                                                    | *
 * | DESCRIPTION: UNSUCCESSFUL QUEUING PORT CREATION UPON AN EXISTING   | *
 * | PARTITION IN NORMAL STATE.                                         | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    09/10/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED API-INTER-0570:0080                                    | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_570_0013.h"


/* ------------------25/11/2002------------------------------------------ +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_INTER_570_0013                | *
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
MasterProcedure_T_API_INTER_570_0013 ( TEST_RESULT_TYPE *TestResult )
{
  BOOLEAN                     FlagOK     = TRUE; 
  RETURN_CODE_TYPE            RetCode    = NO_ERROR;
  QUEUING_PORT_ID_TYPE        PortId     = 0;
  CREATE_QUEUING_PORT_ERROR   PortError;
  QUEUING_PORT_STATUS_TYPE    PortStatus;

  printf("MasterProcedure_T_API_INTER_570_0013\n");

  PortError =  CheckCreateQueuingPort( "QP_ERROR_HANDLING",
 				                           QUEUING_PORT_MAX_MESSAGE_SIZE, 
 				                           QUEUING_PORT_MAX_MESSAGE_NB, 
				                           SOURCE,
				                           FIFO,
				                           INVALID_MODE,
				                           &PortId,
				                           &PortStatus,
				                           &RetCode );

  /* Evaluate errors */
  EvaluateErrorType( RetCode, &FlagOK, INVALID_MODE );

  if( PortError.ReturnCode     != NO_ERROR ||
      PortError.GetStatus      != NO_ERROR ||
      PortError.MaxMessageSize != NO_ERROR ||
      PortError.MaxNBMessage   != NO_ERROR ||
      PortError.PortDirection  != NO_ERROR )
  {
     FlagOK = FALSE;
  }
  
  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );

} /* MasterProcedure_T_API_INTER_570_0013 */



/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_INTER_570_0013.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
