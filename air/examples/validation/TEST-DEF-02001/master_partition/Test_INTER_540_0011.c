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
 * | FILE: Test_INTER_540_0011.c                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): MasterProcedure_T_API_INTER_540_0011        | *
 * |                                                                    | *
 * | DESCRIPTION: SUCCESSFUL SAMPLING MESSAGE READOUT.                  | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    18/10/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0540:010, API-INTER-0540:020,API-INTER-0540 | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_540_0011.h"


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: MasterProcedure_T_API_INTER_540_0011                | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   18/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCESS or FAILED | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
MasterProcedure_T_API_INTER_540_0011( TEST_RESULT_TYPE *TestResult )
{
  RETURN_CODE_TYPE            RetCode  = NO_ERROR;
  BOOLEAN                     FlagOK   = TRUE;
  BOOLEAN                     AuxPartitionStatus  = TRUE;
  SAMPLING_PORT_ID_TYPE       PortId   = 0;
  MESSAGE_SIZE_TYPE           Length  = 0;
  APEX_BYTE                   OutMessage[256];
  VALIDITY_TYPE               Validity = VALID;
  SAMPLING_PORT_STATUS_TYPE   PortStatus;
  READ_SAMPLING_MESSAGE_ERROR ReadError;
    

  printf("MasterProcedure_T_API_INTER_540_0011\n");

  /* Send subtestid to AuxPartition */
  RetCode = SendSubtestIdToAuxPartition( T_API_INTER_540_0011 );
  EvaluateError( RetCode, &FlagOK );

  /* Get port id */
  w_GET_SAMPLING_PORT_ID ( "SP_MASTER_RECEIVE", &PortId, &RetCode );
  EvaluateError( RetCode, &FlagOK );

   memset (OutMessage, 256, '\0');
  
  RetCode = ReadResultsFromAuxPartition( &AuxPartitionStatus );
  EvaluateError( RetCode, &FlagOK );
  EvaluateVariable ( AuxPartitionStatus, TRUE, &FlagOK );

  if(FlagOK == FALSE) printf("1. FlagOK == FALSE\n");
  else printf("1. FlagOK == TRUE\n");


  /* Read message send by AuxPartition */
  ReadError = CheckReadSamplingMessage( PortId,
		   			                      NO_ERROR,
					                         "sampling_message_test",
					                         21,
					                         OutMessage,
					                         &Length,
					                         &Validity,
					                         &PortStatus,
					                         &RetCode,
					                         NO_LOG_ERROR );

  if ( ( ReadError.ReturnCode   != NO_ERROR ) ||
       ( ReadError.MessageSize  != NO_ERROR ) ||
       ( ReadError.Message      != NO_ERROR )    ||
       ( ReadError.GetStatus    != NO_ERROR ) ||
       ( Validity != VALID	) )
  {
	  printf("FALSE\n");
     FlagOK = FALSE;
  }


  if(FlagOK == FALSE) printf("2. FlagOK == FALSE\n");
    else printf("2. FlagOK == TRUE\n");

/*  printf("ReadError.ReturnCode: %d\n", ReadError.ReturnCode);
  printf("ReadError.MessageSize: %d\n", ReadError.MessageSize);
  printf("ReadError.Message: %d\n", ReadError.Message);
  printf("ReadError.GetStatus: %d\n", ReadError.GetStatus);
 */
  if( Validity != VALID	) printf("invalid!\n");
  else printf("valid\n ");

  printf("waiting..\n");
  rtems_task_wake_after(SAMPLING_PORT_REFRESH_TICKS +1);

  /* Delay to avoid cycle to consume to many resources*/
  //TIMED_WAIT (SAMPLING_PORT_REFRESH_TIME, &RetCode);

  //EvaluateError( RetCode, &FlagOK );

  if(FlagOK == FALSE) printf("3. FlagOK == FALSE\n");
    else printf("3. FlagOK == TRUE\n");

  /* Read message send by AuxPartition */
  ReadError = CheckReadSamplingMessage( PortId,
		   			                      NO_ERROR,
					                         "sampling_message_test",
					                         21,
					                         OutMessage,
					                         &Length,
					                         &Validity,
					                         &PortStatus,
					                         &RetCode,
					                         NO_LOG_ERROR );

  if ( ( ReadError.ReturnCode   != NO_ERROR ) ||
       ( ReadError.MessageSize  != NO_ERROR ) ||
       ( ReadError.Message      != NO_ERROR )    ||
       ( ReadError.GetStatus    != NO_ERROR ) ||
       ( Validity != INVALID	) )
  {
	  printf("FALSE\n");
     FlagOK = FALSE;
  }


  
  if(FlagOK == FALSE) printf("4. FlagOK == FALSE\n");
    else printf("4. FlagOK == TRUE\n");

  printf("ReadError.ReturnCode: %d\n", ReadError.ReturnCode);
   printf("ReadError.MessageSize: %d\n", ReadError.MessageSize);
   printf("ReadError.Message: %d\n", ReadError.Message);
   printf("ReadError.GetStatus: %d\n", ReadError.GetStatus);
   if( Validity != VALID	) printf("invalid!\n");
   else printf("valid\n ");

   if(FlagOK == FALSE) printf("FlagOK == FALSE\n");
   else printf("FlagOK == TRUE\n");

  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );

} /* MasterProcedure_T_API_INTER_540_0011  */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_INTER_540_0011.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
