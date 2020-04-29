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
 * | FILE: Aux_INTER_590_0010.c                                         | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): AuxProcedure_T_API_INTER_590_0010           | *
 * |                                                                    | *
 * | DESCRIPTION: TEST THE SERVICE RECEIVE_QUEUING_MESSAGE WHEN THE FIFO| *
 * | QUEUE IS NOT EMPTY AND LENGTH OF MESSAGE IS <= MAXIMUM_LENGTH      | *
 * | ( INPUT PARAMETER OF THE SERVICE ).                                | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    21/10/2002                                                | *
 * |                                                                    | *
 * | CVRS COVERED: API-INTER-590:0010, API-INTER-590                    | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Aux_INTER_590_0010.h"

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: AuxProcedure_T_API_INTER_590_0010                   | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   21/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | int *TestResult                 X      X     The Result of the test| *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
AuxProcedure_T_API_INTER_590_0010(  BOOLEAN *TestResult  )
{
  BOOLEAN           FlagOK  = TRUE;
  RETURN_CODE_TYPE  RetCode = NO_ERROR;
  APEX_BYTE         Message[QUEUING_PORT_MAX_MESSAGE_SIZE];

  /* Compose message */
  memset( Message, '\0', sizeof( Message ));
  strcpy( Message, "Hello world" );

  /* send message to Master Partition */
  RetCode = SendQueuingMessage ( "QP_AUX_SEND", 
				 Message, 
				 strlen(Message),
				 0, 
				 NO_ERROR );

  /* Evaluate errors */
  EvaluateError( RetCode, &FlagOK );

  *TestResult = FlagOK;
  
  WriteResultsFromAuxPartition ( *TestResult );

} /* AuxProcedure_T_API_INTER_590_0010 */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Aux_INTER_590_0010.c                    | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
