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
 * | FILE: Test_PART_230_0050.c                                         | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_PART                                           | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): Test_T_API_PART_230_0050                    | *
 * |                                                                    | *
 * | DESCRIPTION: TEST THE SERVICE SET_PARTITION_MODE WHEN THE          | *
 * | OPERATING MODE DOES NOT REPRESENT AN EXISTING MODE                 | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    16/09/2004                                                | *
 * |                                                                    | *
 * | CVRS COVERED: API-PART-230:0050                                    | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#include "Test_PART_230_0050.h"


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: Test_T_API_PART_230_0050                            | *
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
void MasterProcedure_T_API_PART_230_0050( TEST_RESULT_TYPE *TestResult )
{  
  RETURN_CODE_TYPE           RetCode   = NO_ERROR;
  PARTITION_STATUS_TYPE      PrtStatus;
  SET_PARTITION_MODE_ERROR   PrtError;
  BOOLEAN                    FlagOK;
  int                       InvalidEnumValue;
  
  printf("MasterProcedure_T_API_PART_230_0050\n");

  /* Inicialite FlagOK */
  FlagOK = TRUE;

  /*LANGUAGE DEPENDENT*/
  if ( GetInvalidEnumeratedValue ( &InvalidEnumValue ) == DEPENDENCE_AVAILABLE )
  {
     /* Set partition mode */
     PrtError = CheckSetPartitionMode( INVALID_ENUMERATED_VALUE, 
		                                 INVALID_PARAM, &PrtStatus, 
		                                 &RetCode );

     /* Evaluate errors */
     EvaluateErrorType( RetCode, &FlagOK, INVALID_PARAM );
     if ( ( PrtError.ReturnCode != NO_ERROR ) || 
			 ( PrtError.GetStatus  != NO_ERROR ) ||
    	    ( PrtStatus.OPERATING_MODE != NORMAL ) )
			  
     {
        FlagOK = FALSE;
     }
  }
  /*END LANGUAGE DEPENDENT*/

  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );

} /* MasterProcedure_T_API_PART_230_0050 */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_PART_230_0050.c                    | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
