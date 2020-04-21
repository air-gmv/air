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
 * | FILE: MasterProcedure_T_API_PART_230_0060                          | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_PART                                           | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): MasterProcedure_T_API_PART_230_0060         | *
 * |                                                                    | *
 * | DESCRIPTION: TEST THE SERVICE SET_PARTITION_MODE TO WARM_START     | *
 * | COMING FROM STATE COLD_START                                       | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    16/09/2004                                                | *
 * |                                                                    | *
 * | CVRS COVERED: API-PART-230:0060                                    | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#include "Test_PART_230_0060.h"

extern int TraceActive;

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_PART_230_0060                 | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   16/09/2004                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCESS or FAILED | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
MasterProcedure_T_API_PART_230_0060 ( TEST_RESULT_TYPE *TestResult )
{  
  RETURN_CODE_TYPE           RetCode  = NO_ERROR;
  BOOLEAN                    FlagOK   = TRUE;
  PARTITION_STATUS_TYPE      PrtStatus;
  SET_PARTITION_MODE_ERROR   PrtError;

  printf("MasterProcedure_T_API_PART_230_0060\n");

  /* Set partition mode */
  PrtError = CheckSetPartitionMode( WARM_START, INVALID_MODE, &PrtStatus, &RetCode );

  /* Evaluate errors */
  EvaluateErrorType( RetCode, &FlagOK, INVALID_MODE );
  if ( ( PrtError.ReturnCode != NO_ERROR ) ||
       ( PrtError.GetStatus  != NO_ERROR ) ||
	    ( PrtStatus.OPERATING_MODE != COLD_START ) )
  {
     FlagOK = FALSE;
  }
  
  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );

} /* MasterProcedure_T_API_PART_230_0060 */

/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_PART_230_0060.c                    | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
