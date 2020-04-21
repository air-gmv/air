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
 * | FILE: Test_INTER_520_0012.c                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): Test_T_API_INTER_520_0012                   | *
 * |                                                                    | *
 * | DESCRIPTION: SAMPLING PORT CREATION ERROR HANDLING.                | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    08/10/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0520:0030, API-INTER-0520:0050,             | *
 * |              API-INTER-0520:0070, API-INTER-0520:0020,             | *
 * |              API-INTER-0520:0060                                   | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_520_0012.h"

extern int TraceActive;

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: Test_T_API_INTER_520_0012                           | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   08/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCESS or FAILED | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
Test_T_API_INTER_520_0012( TEST_RESULT_TYPE *TestResult )
{
  BOOLEAN                     FlagOK         = TRUE;
  RETURN_CODE_TYPE            RetCode        = NO_ERROR;
  SAMPLING_PORT_ID_TYPE       SamplingPortId = 0;
  CREATE_SAMPLING_PORT_ERROR  PortError;
  SAMPLING_PORT_STATUS_TYPE   PortStatus;
  int                         InvalidEnumeratedValue;
  SYSTEM_TIME_TYPE            InvalidTimeValue;
  MESSAGE_SIZE_TYPE           InvalidMessageSizeValue;


  printf("Test_T_API_INTER_520_0012\n");

  /* 1 - Try to create a sampling port with a invalid NAME  */

  PortError =  CheckCreateSamplingPort( "INVALID_NAME", 
					                       SAMPLING_PORT_MAX_MESSAGE_SIZE, 
                                      SOURCE, 
                                      SAMPLING_PORT_REFRESH_TIME, 
                                      INVALID_CONFIG, 
                                      &SamplingPortId, 
                                      &PortStatus, 
                                      &RetCode );
   /* To evaluate errors */
   EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );

   if(PortError.ReturnCode     != NO_ERROR || 
      PortError.GetStatus      != NO_ERROR || 
      PortError.MaxMessageSize != NO_ERROR || 
      PortError.PortDirection  != NO_ERROR || 
      PortError.RefreshPeriod  != NO_ERROR ) 
   { 
      FlagOK = FALSE; 
   } 


  /* 2 - Try to create a sampling port with the incorrect port direction */
  PortError =  CheckCreateSamplingPort( "SP_ERROR_HANDLING",
					SAMPLING_PORT_MAX_MESSAGE_SIZE,
					DESTINATION,
					SAMPLING_PORT_REFRESH_TIME,
					INVALID_CONFIG,
					&SamplingPortId,
					&PortStatus,
					&RetCode );
  /* To evaluate errors */
  EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );

  if(PortError.ReturnCode     != NO_ERROR ||
     PortError.GetStatus      != NO_ERROR ||
     PortError.MaxMessageSize != NO_ERROR ||
     PortError.PortDirection  != NO_ERROR ||
     PortError.RefreshPeriod  != NO_ERROR )
  {
     FlagOK = FALSE;
  }

  /* 3 - Try to create a sampling port with an incorrect port size*/
  PortError = CheckCreateSamplingPort( "SP_ERROR_HANDLING",
				       SAMPLING_PORT_MAX_MESSAGE_SIZE - 1,
				       SOURCE,
				       SAMPLING_PORT_REFRESH_TIME,
				       INVALID_CONFIG,
				       &SamplingPortId,
				       &PortStatus,
				       &RetCode );
  /* To evaluate errors */
  EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );

  if(PortError.ReturnCode     != NO_ERROR ||
     PortError.GetStatus      != NO_ERROR ||
     PortError.MaxMessageSize != NO_ERROR ||
     PortError.PortDirection  != NO_ERROR ||
     PortError.RefreshPeriod  != NO_ERROR )
  {
     FlagOK = FALSE;
  }

  /*LANGUAGE DEPENDENT TEST*/ 
  
  /* 2.1 - Try to create a sampling port with the invalid port direction */
  if ( GetInvalidEnumeratedValue ( &InvalidEnumeratedValue ) == DEPENDENCE_AVAILABLE )
  {
     PortError =  CheckCreateSamplingPort( "SP_ERROR_HANDLING",
		  			   SAMPLING_PORT_MAX_MESSAGE_SIZE,
					   InvalidEnumeratedValue,
					   SAMPLING_PORT_REFRESH_TIME,
					   INVALID_CONFIG,
					   &SamplingPortId,
					   &PortStatus,
					   &RetCode );
	  
     /* To evaluate errors */
     EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );

    if ( PortError.ReturnCode     != NO_ERROR ||
         PortError.GetStatus      != NO_ERROR ||
         PortError.MaxMessageSize != NO_ERROR ||
         PortError.PortDirection  != NO_ERROR ||
         PortError.RefreshPeriod  != NO_ERROR )
    {
       FlagOK = FALSE;
    }
 }

/*END OF LANGUAGE DEPENDENT TEST*/ 
  
   /* 4.1 - Try to create a sampling port with an incorrect refresh period*/
   PortError = CheckCreateSamplingPort( "SP_ERROR_HANDLING",
	  				    SAMPLING_PORT_MAX_MESSAGE_SIZE,
					    SOURCE,
					    SAMPLING_PORT_REFRESH_TIME - 1,
					    INVALID_CONFIG,
					    &SamplingPortId,
					    &PortStatus,
					    &RetCode );
   /* To evaluate errors */
   EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );

   if(PortError.ReturnCode     != NO_ERROR ||
      PortError.GetStatus      != NO_ERROR ||
      PortError.MaxMessageSize != NO_ERROR ||
      PortError.PortDirection  != NO_ERROR ||
      PortError.RefreshPeriod  != NO_ERROR )
   {
      FlagOK = FALSE; 
   }


/*IMPLEMENTATION DEPENDENT TESTS*/  
  
  /* 4.2 - Try to create a sampling port with an invalid REFRESH PERIOD*/
  if ( GetInvalidTimeValue ( &InvalidTimeValue ) == DEPENDENCE_AVAILABLE )
  {
     PortError = CheckCreateSamplingPort( "SP_ERROR_HANDLING",
		   		                           SAMPLING_PORT_MAX_MESSAGE_SIZE,
				                              SOURCE,
				                              InvalidTimeValue,
				                              INVALID_CONFIG,
				                              &SamplingPortId,
				                              &PortStatus,
				                              &RetCode );
     /* To evaluate errors */
     EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );

     if ( PortError.ReturnCode     != NO_ERROR ||
          PortError.GetStatus      != NO_ERROR ||
          PortError.MaxMessageSize != NO_ERROR ||
          PortError.PortDirection  != NO_ERROR ||
          PortError.RefreshPeriod  != NO_ERROR )
     {
        FlagOK = FALSE;
     }
  }

  /* 3.1 - Try to create a sampling port with an invalid port size*/
  if ( GetInvalidMessageSizeValue ( &InvalidMessageSizeValue ) == DEPENDENCE_AVAILABLE )
  {
     PortError = CheckCreateSamplingPort( "SP_ERROR_HANDLING",
				                              InvalidMessageSizeValue,
				                              SOURCE,
				                              SAMPLING_PORT_REFRESH_TIME,
				                              INVALID_CONFIG,
				                              &SamplingPortId,
				                              &PortStatus,
				                              &RetCode );
     /* To evaluate errors */
     EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );

     if ( PortError.ReturnCode     != NO_ERROR ||
          PortError.GetStatus      != NO_ERROR ||
          PortError.MaxMessageSize != NO_ERROR ||
          PortError.PortDirection  != NO_ERROR ||
          PortError.RefreshPeriod  != NO_ERROR )
    {
       FlagOK = FALSE;
    }
  }

  *TestResult = EvaluateResult( FlagOK );
  return;


  /* 6 - Try create a sampling port, exceeding the  */
  /*     MAX_NUMBER_OF_SAMPLING_PORTS ( allowed sampling port number */

  if ( DoSamplingPortExaustion ( &FlagOK ) == DEPENDENCE_AVAILABLE )
  {
     PortError = CheckCreateSamplingPort ( "SP_OUT_OF_RANGE" ,
			                                  SAMPLING_PORT_MAX_MESSAGE_SIZE,
			                                  DESTINATION,
			                                  SAMPLING_PORT_REFRESH_TIME,
			                                  INVALID_CONFIG,
			                                  &SamplingPortId,
			                                  &PortStatus,
			                                  &RetCode );
  
      /* To evaluate errors */
      EvaluateErrorType ( RetCode, &FlagOK, INVALID_CONFIG );

      if ( PortError.ReturnCode     != NO_ERROR ||
           PortError.GetStatus      != NO_ERROR ||
           PortError.MaxMessageSize != NO_ERROR ||
           PortError.PortDirection  != NO_ERROR ||
           PortError.RefreshPeriod  != NO_ERROR )
      {
         FlagOK = FALSE;
      }
   }
/*END OF IMPLEMENTATION DEPENDENT TESTS*/  
	
   /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );
  
} /* Test_T_API_INTER_520_0012 */



/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_INTER_520_0012.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
