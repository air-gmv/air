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
 * | FILE: Test_INTER_590_0070.c                                         | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): MasterProcedure_T_API_INTER_590_0070         | *
 * |                                                                    | *
 * | DESCRIPTION: TEST THE SERVICE RECEIVE_QUEUING_MESSAGE UNDER ERROR  | *
 * | CONDITIONS.                                                        | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    24/10/2002                                                | *
 * |                                                                    | *
 * | CVRS COVERED: API-INTER-590:0080, API-INTER-590:0090,              | *
 * | API-INTER-590:0100,                                                | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_590_0070.h"


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_INTER_590_0070                 | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   24/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCESS or FAILED | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
MasterProcedure_T_API_INTER_590_0070( TEST_RESULT_TYPE *TestResult )
{
  RETURN_CODE_TYPE               RetCode = NO_ERROR;
  BOOLEAN                        FlagOK  = TRUE;
  QUEUING_PORT_ID_TYPE           PortId  = 0;
  APEX_BYTE                      Message[64];
  MESSAGE_SIZE_TYPE              Length;
  QUEUING_PORT_STATUS_TYPE       PortStatus;
  RECEIVE_QUEUING_MESSAGE_ERROR  PortError;
  SYSTEM_TIME_TYPE               InvalidTimeValue;

printf("MasterProcedure_T_API_INTER_590_0070\n");
  /* Clean received area */
  memset( Message, '\0', sizeof( Message ));

  /* 1 -Try read message from a non existent queuing port */
  PortError = CheckReceiveQueuingMessage( 10000,
            0,    /* timeout */
            "",
            0,
            INVALID_PARAM,
            Message,
            &Length,
            &PortStatus,
            &RetCode );
  /* Evaluate errors */
  EvaluateErrorType( RetCode, &FlagOK, INVALID_PARAM );

  if( PortError.ReturnCode   != NO_ERROR ||
      PortError.MessageSize  != NO_ERROR ||
      PortError.Message      != NO_ERROR ||
      PortError.GetStatus    != NO_ERROR )
  {
    FlagOK = FALSE;
  }


 /* Get "QP_MASTER_RECEIVE" port id */
  w_GET_QUEUING_PORT_ID ( "QP_MASTER_RECEIVE", &PortId, &RetCode );
  EvaluateError( RetCode, &FlagOK );

  /* 2 - Try read message from queuing port with out of range timeout*/
  /* Test is comented because in this version there is no invalid timeout value*/
  if ( GetInvalidTimeValue ( &InvalidTimeValue) == DEPENDENCE_AVAILABLE )
  {
     PortError = CheckReceiveQueuingMessage( PortId,
                                            InvalidTimeValue,        
                                            "",
                                            0,
                                            INVALID_PARAM,
                                            Message,
                                            &Length,
                                            &PortStatus,
                                            &RetCode );
				
     /* Evaluate errors */
     EvaluateErrorType( RetCode, &FlagOK, INVALID_PARAM );

     if ( PortError.ReturnCode   != NO_ERROR ||
          PortError.MessageSize  != NO_ERROR ||
          PortError.Message      != NO_ERROR ||
          PortError.GetStatus    != NO_ERROR )
     {
       FlagOK = FALSE;
     }
  }  
  
 /* Get "QP_MASTER_SEND" port id */
  w_GET_QUEUING_PORT_ID ( "QP_MASTER_SEND", &PortId, &RetCode );
  EvaluateError( RetCode, &FlagOK );

  /* 3 - Try read message from queuing port that is a source*/
  PortError = CheckReceiveQueuingMessage( PortId,
            0,       /* timeout */
            "",
            0,
            INVALID_MODE,
            Message,
            &Length,
            &PortStatus,
            &RetCode );
  /* Evaluate errors */
  EvaluateErrorType( RetCode, &FlagOK, INVALID_MODE );

  if( PortError.ReturnCode   != NO_ERROR ||
      PortError.MessageSize  != NO_ERROR ||
      PortError.Message      != NO_ERROR ||
      PortError.GetStatus    != NO_ERROR )
  {
    FlagOK = FALSE;
  }
  
  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );


} /* MasterProcedure_T_API_INTER_590_0070 */



/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_INTER_590_0070.c                    | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
