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
 * | FILE: Aux_INTER_540_0011.c                                         | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): AuxProcedure_T_API_INTER_540_0011           | *
 * |                                                                    | *
 * | DESCRIPTION: SUCCESSFUL SAMPLING MESSAGE READOUT.                  | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    18/10/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0540:010, API-INTER-0540                    | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Aux_INTER_540_0011.h"

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: AuxProcedure_T_API_INTER_540_0011                   | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   18/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
AuxProcedure_T_API_INTER_540_0011( BOOLEAN *TestResult )
{
  RETURN_CODE_TYPE  RetCode = NO_ERROR;
  APEX_BYTE         Message[256];
  BOOLEAN           FlagOK  = TRUE;
  

  printf("AuxProcedure_T_API_INTER_540_0011\n");
  /* Compose message to send for AuxPartition */
  strcpy( Message, "sampling_message_test" );
  
  /* Send message to master partition */
  RetCode = WriteSamplingMessage( "SP_AUX_SEND", Message, strlen (Message));
  /* evaluate errors */
  EvaluateError( RetCode, &FlagOK );

  WriteResultsFromAuxPartition ( FlagOK );

  *TestResult = FlagOK;

}  /* AuxProcedure_T_API_INTER_540_0011  */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Aux_INTER_540_0011.c                    | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
