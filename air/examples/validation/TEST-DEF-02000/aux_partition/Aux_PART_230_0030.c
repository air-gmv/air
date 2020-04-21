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
 * | FILE: Aux_PART_230_0030.c                                          | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AUX_API_PART                                            | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): AuxProcedure_T_API_PART_230_0030            | *
 * |                        SubAuxP1_T_API_PART_230_0030                | *
 * |                                                                    | *
 * | DESCRIPTION: TEST THE SERVICE SET_PARTITION_MODE TO WARM_START.    | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    04/10/2002                                                | *
 * |                                                                    | *
 * | CVRS COVERED: API-PART-230:0030                                    | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#include "Aux_PART_230_0030.h"

extern int TraceActive;

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: AuxProcedure_T_API_PART_230_0030                    | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   04/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:  int *TestResult with OK our FAIL                    | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
AuxProcedure_T_API_PART_230_0030( BOOLEAN *TestResult )
{
  RETURN_CODE_TYPE         RetCode = NO_ERROR;

  printf("AuxProcedure_T_API_PART_230_0030\n");

  /* call w_SET_PARTITION_MODE procedure */
  w_SET_PARTITION_MODE( WARM_START, &RetCode );

}  /* AuxProcedure_T_API_PART_230_0030 */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Aux_PART_230_0030.c                     | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
