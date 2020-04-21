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
 * | FILE: AVT_Partition.c                                              | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: PARTITION                                               | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S):   CheckGetPartitionStatus                   | *
 * |                          CheckSetPartitionMode                     | *
 * |                          CheckGetPartitionStartCondition  (NEW)    | *
 * |                                                                    | *
 * | DESCRIPTION:   The goal of these functions, is to test the similar | *
 * |                functions of ARINC653,  passing them the input      | *
 * |                parameters of the ARINC653 functions and the        | *
 * |                expected values for the output parameters and       | *
 * |                verified that the obtained  values are the expected | *
 * |                ones.                                               | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   28/08/2002                                                 | *
 * |                                                                    | *
 * | FTP REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "AVT_Partition.h"


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckSetPartitionMode                               | *
 * |                                                                    | *
 * | DESCRIPTION:  Execute the ARINC653 function SET_PARTITION_MODE,    | *
 * |               and compares the obtained values against the expected| *
 * |               values. If  they are differents fills the error      | *
 * |               output error variable.                               | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   28/08/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | OPERATING_MODE InOperMode       X            variable  with the    | *
 * |                                              value  to pass to the | *
 * |                                              the function.         | *
 * | RETURN_CODE_TYPE InReturnCode   X            Expected return code, | *
 * |                                              resulting from the    | *
 * |                                              call of the function  | *
 * |                                              under test.           | *
 * | PARTITION_STATUS_TYPE *OutStatus        X    Structure filled with | *
 * |                                              information regarding | *
 * |                                              the status of the     | *
 * |                                              operation.            | *
 * | RETUEN_CODE_TYPE *OutReturnCode         X    Return code of the    | *
 * |                                              ARINC function that   | *
 * |                                              was called.           | *
 * | RETURN VALUE:                                                      | *
 * | SET_PARTITION_MODE_ERROR                     Bit structure holding | *
 * |                                              the different types of| *
 * |                                              errors that can occur | *
 * |                                              where the error that  | *
 * |                                              occurred has its bit  | *
 * |                                              set to one. The others! *
 * |                                              are set to 0.         | *
 * ---------------------------------------------------------------------- */
SET_PARTITION_MODE_ERROR
CheckSetPartitionMode( OPERATING_MODE_TYPE    InOperMode,
		       RETURN_CODE_TYPE       InReturnCode,
		       PARTITION_STATUS_TYPE *OutStatus,
		       RETURN_CODE_TYPE      *OutReturnCode )
{
  SET_PARTITION_MODE_ERROR ReturnError;
  RETURN_CODE_TYPE         ReturnCode = NO_ERROR;

  /* Clean all output parameters */
  memset( (void*)OutStatus, '\0', sizeof(PARTITION_STATUS_TYPE));
  *OutReturnCode = NO_ERROR;

  /* Clean error bit struct */
  ReturnError.ReturnCode = NO_ERROR;
  ReturnError.GetStatus  = NO_ERROR;

  /* Call ARINC653 function */
  w_SET_PARTITION_MODE( InOperMode, OutReturnCode );

  /* Evaluate result */

  /* RETURN CODE */
  if( InReturnCode != *OutReturnCode )
  {
    ReturnError.ReturnCode = AVT_ERROR;
    WriteLogReturnCode("CkeckSetPartitionMode", InReturnCode, *OutReturnCode );
  }

  /* Executes GET_PARTITION_STATUS and return the status  */
  w_GET_PARTITION_STATUS( OutStatus, &ReturnCode );
  if( ReturnCode != NO_ERROR )
  {
    ReturnError.GetStatus = AVT_ERROR;
    WriteLogErrorFunc( ReturnCode, 
		       "CkeckSetPartitionMode", 
		       "GET_PARTITION_STATUS" );
  }
  return ReturnError;

} /* CheckSetPartitionMode */




/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                    END OF FILE: AVT_Partition.c                    | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
