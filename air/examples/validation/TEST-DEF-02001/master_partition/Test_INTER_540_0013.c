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
 * | FILE: Test_INTER_540_0013.c                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): MasterProcedure_T_API_INTER_540_0013        | *
 * |                                                                    | *
 * | DESCRIPTION: SUCCESSFUL SAMPLING MESSAGE READOUT.                  | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    26/08/2004                                                | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0540:030                                    | *
 * |                                                                    | *
 * |                                                                    | *
 * |Note: for this test to execute correctly the used sampling port must| *
 * |      never had received a messsage                                 | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_540_0013.h"


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_INTER_540_0013                | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   26/08/2004                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCESS or FAILED | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
MasterProcedure_T_API_INTER_540_0013( TEST_RESULT_TYPE *TestResult )
{
  RETURN_CODE_TYPE            RetCode  = NO_ERROR;
  BOOLEAN                     FlagOK   = TRUE;
  SAMPLING_PORT_ID_TYPE       PortId   = 0;
  MESSAGE_SIZE_TYPE           Length  = 0;
  APEX_BYTE                   Message[256];
  VALIDITY_TYPE               Validity = VALID;
  SAMPLING_PORT_STATUS_TYPE   PortStatus;
  READ_SAMPLING_MESSAGE_ERROR ReadError;

  printf("MasterProcedure_T_API_INTER_540_0013\n");
    
  memset (Message, 256, '\0');
  /* Get port id */
  w_GET_SAMPLING_PORT_ID ( "SP_MASTER_RECEIVE", &PortId, &RetCode );

  EvaluateError( RetCode, &FlagOK );


  /* Read message send by AuxPartition */
  ReadError = CheckReadSamplingMessage( PortId,
		   			  NO_ACTION,
					     "",
					     0,
					     Message,
					     &Length,
					     &Validity,
					     &PortStatus,
					     &RetCode,
					     NO_LOG_ERROR );
						  
  /* Evaluate results */
	  
  /*Get Time of Reading*/
     
  if ( ( ReadError.ReturnCode   != NO_ERROR ) ||
       ( ReadError.MessageSize  != NO_ERROR ) ||
       ( ReadError.Message      != NO_ERROR ) ||
       ( ReadError.GetStatus    != NO_ERROR ) ||
       ( Validity != INVALID	) )
  {
     FlagOK = FALSE;
  }

   /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );

} /* MasterProcedure_T_API_INTER_540_0013  */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_INTER_540_0013.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
