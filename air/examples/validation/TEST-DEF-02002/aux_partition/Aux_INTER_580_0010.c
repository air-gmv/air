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
 * | FILE: Aux_INTER_580_0010.c                                         | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): AuxProcedure_T_API_INTER_580_0010           | *
 * |                                                                    | *
 * | DESCRIPTION: TEST THE SERVICE SEND_QUEING_MESSAGE WITH ENOUGH SPACE| *
 * | AND NO PROCESSES WAITING                                           | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    21/10/2002                                                | *
 * |                                                                    | *
 * | CVRS COVERED: API- INTER -580:0010, API- INTER -580                | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Aux_INTER_580_0010.h"

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: AuxProcedure_T_API_INTER_580_0010                   | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   21/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
AuxProcedure_T_API_INTER_580_0010(  BOOLEAN *TestResult  )
{
  RETURN_CODE_TYPE RetCode = NO_ERROR;
  BOOLEAN          FlagOK  = TRUE;
  APEX_BYTE        Message[64];

  printf("AuxProcedure_T_API_INTER_580_0010\n");
  /* Wait for a message came from Master partition */
  RetCode = ReceiveQueuingMessage ( "QP_AUX_RECEIVE",
				    INFINITE_TIME_VALUE,
				    Message );


  /* Evaluate errors */
  EvaluateError( RetCode, &FlagOK );

  *TestResult = FlagOK;
  
  WriteResultsFromAuxPartition ( *TestResult );

} /* AuxProcedure_T_API_INTER_580_0010 */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Aux_INTER_580_0010.c                    | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
