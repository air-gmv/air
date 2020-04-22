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
 * | FILE: Aux_INTER_530_0011.c                                         | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): AuxProcedure_T_API_INTER_530_0011           | *
 * |                                                                    | *
 * | DESCRIPTION:  SUCCESSFUL SAMPLING MESSAGE WRITE.                   | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    18/10/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0530:010, API-INTER-0530                    | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Aux_INTER_530_0011.h"

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: AuxProcedure_T_API_INTER_530_0011                   | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   18/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | int *TestResult                 X      X     The result of the test| *
 * |                                              OK or FAIL.           | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
AuxProcedure_T_API_INTER_530_0011( BOOLEAN *TestResult )
{
  RETURN_CODE_TYPE            RetCode  = NO_ERROR;
  SAMPLING_PORT_ID_TYPE       PortId   = 0;
  MESSAGE_SIZE_TYPE           Length   = 0;
  VALIDITY_TYPE               Validity = VALID;
  APEX_BYTE                   Message[256];
  APEX_BYTE                   InMessage[256];
  SAMPLING_PORT_STATUS_TYPE   PortStatus;
  READ_SAMPLING_MESSAGE_ERROR ReadError;

  printf("AuxProcedure_T_API_INTER_530_0011\n");

  /* initialize result */
  *TestResult = TRUE;

  /* compose expected message */
  strcpy( InMessage, "sampling_message_test" );

  /* get portid and readed it */
  w_GET_SAMPLING_PORT_ID( "SP_AUX_RECEIVE", &PortId, &RetCode );



  printf("read smsg from SP_AUX_RECEIVE InMessage: %s\n", InMessage);
  ReadError = CheckReadSamplingMessage( PortId,
					NO_ERROR,
					InMessage,
					strlen( InMessage ),
					Message,
					&Length,
					&Validity,
					&PortStatus,
					&RetCode,
					LOG_ERROR);

  
  
  /* Evaluate errors */
  EvaluateError( RetCode, TestResult );
  if( ReadError.ReturnCode   != NO_ERROR ||
      ReadError.MessageSize  != NO_ERROR ||
      ReadError.Message      != NO_ERROR ||
      ReadError.GetStatus    != NO_ERROR )
  {
     *TestResult = FALSE;
  }
  
  WriteResultsFromAuxPartition ( *TestResult );
    	 
}  /* AuxProcedure_T_API_INTER_530_0011  */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Aux_INTER_530_0011.c                    | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
