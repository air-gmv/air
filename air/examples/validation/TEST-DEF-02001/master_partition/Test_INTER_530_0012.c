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
 * | FILE: Test_INTER_530_0012.c                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): MasterProcedure_T_API_INTER_530_0012        | *
 * |                        SubAuxProcedure_T_API_INTER_530_0012        | *
 * |                                                                    | *
 * | DESCRIPTION: SAMPLING MESSAGE WRITE ERROR HANDLING. (INVALID       | *
 * | PARAMETERS).                                                       | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    18/10/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0530:020, API-INTER-0530:030,               | *
 * |              API-INTER-0530:040                                    | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_530_0012.h"


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_INTER_530_0012                | *
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
MasterProcedure_T_API_INTER_530_0012( TEST_RESULT_TYPE *TestResult )
{
  RETURN_CODE_TYPE             RetCode   = NO_ERROR;
  BOOLEAN                      FlagOK    = TRUE;
  SAMPLING_PORT_ID_TYPE        PortId = 0;
  APEX_BYTE                    Message[SAMPLING_PORT_MAX_MESSAGE_SIZE + 1];
  SAMPLING_PORT_STATUS_TYPE    PortStatus;
  WRITE_SAMPLING_MESSAGE_ERROR WriteError;
printf("MasterProcedure_T_API_INTER_530_0012\n");
  /* compose message */
  memset (Message, 0, sizeof(Message)); 
  strcpy( Message, "sampling_message_test" );

  /*  1 -  Execute CheckWriteSamplingMessage with a invalid id */
  WriteError =  CheckWriteSamplingMessage( -1,
					   Message,
					   strlen( Message ),
					   INVALID_PARAM,
					   &PortStatus,
					   &RetCode );

  /* Evaluate errors */
  EvaluateErrorType( RetCode, &FlagOK, INVALID_PARAM );

  if( WriteError.ReturnCode != NO_ERROR ||
      WriteError.GetStatus  != NO_ERROR )
  {
    FlagOK = FALSE;
  }

  /* API version 2.0: return code is INVALID_PARAM: */
  /* 2 - Execute CheckWriteSamplingMessage with a negative message size */
  w_GET_SAMPLING_PORT_ID( "SP_MASTER_SEND", &PortId, &RetCode );
  EvaluateError( RetCode, &FlagOK );

  memset (Message, 0, sizeof(Message));
  memset (Message, '1', SAMPLING_PORT_MAX_MESSAGE_SIZE + 1); 
  WriteError =  CheckWriteSamplingMessage( PortId,
					   Message,
					   SAMPLING_PORT_MAX_MESSAGE_SIZE + 1,
					   INVALID_CONFIG,
					   &PortStatus,
					   &RetCode );
  
  /* evaluate errors */
  EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );
  if( WriteError.ReturnCode != NO_ERROR ||
      WriteError.GetStatus  != NO_ERROR )
  {
    FlagOK = FALSE;
  }

  /* compose message */
  memset (Message, 0, sizeof(Message));
  strcpy( Message, "sampling_message_test" );

  /* 3 - Execute CheckWriteSamplingMessage from a DESTINATION port */
  w_GET_SAMPLING_PORT_ID( "SP_MASTER_RECEIVE", &PortId, &RetCode );
  EvaluateError( RetCode, &FlagOK );

  WriteError =  CheckWriteSamplingMessage( PortId,
					   Message,
					   strlen( Message ),
					   INVALID_MODE,
					   &PortStatus,
					   &RetCode );
  /* evaluate errors */
  EvaluateErrorType( RetCode, &FlagOK, INVALID_MODE );
  if( WriteError.ReturnCode != NO_ERROR ||
      WriteError.GetStatus  != NO_ERROR )
  {
    FlagOK = FALSE;
  }

   /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );

} /* MasterProcedure_T_API_INTER_530_0012  */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_INTER_530_0012.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
