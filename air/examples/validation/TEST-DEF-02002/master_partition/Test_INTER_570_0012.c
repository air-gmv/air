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
 * | FILE: Test_INTER_570_0012.c                                        | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: TEST_API_INTER                                          | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): Test_T_API_INTER_570_0012                   | *
 * |                                                                    | *
 * | DESCRIPTION: QUEUING PORT CREATION ERROR HANDLING.                 | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    09/10/2002                                                | *
 * |                                                                    | *
 * | CVR COVERED: API-INTER-0570:0030, API-INTER-0570:0030,             | *
 * |  API-INTER-0570:0050, API-INTER-0570:0060, API- INTER -0570:0070   | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "Test_INTER_570_0012.h"

extern int TraceActive;

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: Test_T_API_INTER_570_0012                           | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   09/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                         Input  Output  Description     | *
 * | ENUM TEST_RESULT_TYPE *TestResult           X     Test result      | *
 * |                                                   SUCESS or FAILED | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
Test_T_API_INTER_570_0012( TEST_RESULT_TYPE *TestResult )
{
  BOOLEAN                     FlagOK     = TRUE;
  RETURN_CODE_TYPE            RetCode = NO_ERROR;
  QUEUING_PORT_ID_TYPE        PortId  = 0;
  CREATE_QUEUING_PORT_ERROR   PortError;
  QUEUING_PORT_STATUS_TYPE    PortStatus;
  int                         InvalidEnumeratedValue;
  MESSAGE_SIZE_TYPE           InvalidMessageSizeValue;
  MESSAGE_RANGE_TYPE          InvalidMessageRangeValue;

  printf("Test_T_API_INTER_570_0012\n");

  /* 1.1 - Try to create a queuing port with a INVALID NAME */
  /* name, a INVALID_CONFIG error should be raised */
  PortError = CheckCreateQueuingPort( "INVALID_NAME", 
 				      QUEUING_PORT_MAX_MESSAGE_SIZE, 
 				      QUEUING_PORT_MAX_MESSAGE_NB, 
 				      SOURCE, 
		  		      FIFO, 
 				      INVALID_CONFIG, 
 				      &PortId, 
 				      &PortStatus, 
 				      &RetCode ); 
  /* Evaluate errors */
  EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG ); 
  if( PortError.ReturnCode     != NO_ERROR || 
      PortError.GetStatus      != NO_ERROR || 
      PortError.MaxMessageSize != NO_ERROR || 
      PortError.MaxNBMessage   != NO_ERROR || 
      PortError.PortDirection  != NO_ERROR ) 
   { 
      FlagOK = FALSE; 
   } 

  if(FlagOK == FALSE){
	  printf("1. Test failed\n");
  }

  /* 2.1 - Try to create a queuing port with a non configured port direction   */
   PortError = CheckCreateQueuingPort( "QP_ERROR_HANDLING", 
 				      QUEUING_PORT_MAX_MESSAGE_SIZE, 
 				      QUEUING_PORT_MAX_MESSAGE_NB, 
 				      DESTINATION, 
 				      FIFO, 
 				      INVALID_CONFIG, 
 				      &PortId, 
 				      &PortStatus, 
 				      &RetCode ); 
  /* Evaluate errors */
   EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );
   if( PortError.ReturnCode     != NO_ERROR || 
       PortError.GetStatus      != NO_ERROR || 
       PortError.MaxMessageSize != NO_ERROR || 
       PortError.MaxNBMessage   != NO_ERROR || 
       PortError.PortDirection  != NO_ERROR ) 
   { 
      FlagOK = FALSE; 
   }   

   if(FlagOK == FALSE){
 	  printf("2. Test failed\n");
   }



  /* 2.2 - Try to create a queuing port with an invalid port direction   */
  if ( GetInvalidEnumeratedValue ( &InvalidEnumeratedValue ) == DEPENDENCE_AVAILABLE )
  {
     PortError = CheckCreateQueuingPort( "QP_ERROR_HANDLING",
    				      QUEUING_PORT_MAX_MESSAGE_SIZE, 
 	   			      QUEUING_PORT_MAX_MESSAGE_NB, 
		   		      InvalidEnumeratedValue,
			   	      FIFO,
				         INVALID_CONFIG,
				         &PortId,
				         &PortStatus,
				         &RetCode );
     /* Evaluate errors */
     EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );

     if( PortError.ReturnCode     != NO_ERROR ||
         PortError.GetStatus      != NO_ERROR ||
         PortError.MaxMessageSize != NO_ERROR ||
         PortError.MaxNBMessage   != NO_ERROR ||
         PortError.PortDirection  != NO_ERROR )
     {
        FlagOK = FALSE;
     }


     if(FlagOK == FALSE){
   	  printf("3. Test failed\n");
     }

  }  


  /* 3 - Try to create a queuing port with an invalid queuing discipline */
  if ( GetInvalidEnumeratedValue ( &InvalidEnumeratedValue ) == DEPENDENCE_AVAILABLE )
  {
      PortError = CheckCreateQueuingPort( "QP_ERROR_HANDLING", 
 	   			      QUEUING_PORT_MAX_MESSAGE_SIZE, 
 		   		      QUEUING_PORT_MAX_MESSAGE_NB, 
    				      SOURCE, 
 	   			      InvalidEnumeratedValue, 
 		   		      INVALID_CONFIG, 
 			   	      &PortId, 
 				         &PortStatus, 
 				         &RetCode ); 
     /* Evaluate errors */
      EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG ); 
      if( PortError.ReturnCode     != NO_ERROR || 
          PortError.GetStatus      != NO_ERROR || 
          PortError.MaxMessageSize != NO_ERROR || 
          PortError.MaxNBMessage   != NO_ERROR || 
          PortError.PortDirection  != NO_ERROR ) 
      { 
         FlagOK = FALSE;
      }

      if(FlagOK == FALSE){
    	  printf("4. Test failed\n");
      }

	}   

  /* 4.1 - Try to create a queuing port with a MAX_MESSAGE_SIZE */
  /*       value incompatible with configuration  */
  PortError =  CheckCreateQueuingPort( "QP_ERROR_HANDLING",
 				       QUEUING_PORT_MAX_MESSAGE_SIZE - 1, 
 				       QUEUING_PORT_MAX_MESSAGE_NB, 
				       SOURCE,
				       FIFO,
				       INVALID_CONFIG,
				       &PortId,
				       &PortStatus,
				       &RetCode );
  /* Evaluate errors */
  EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );
  if( PortError.ReturnCode     != NO_ERROR ||
      PortError.GetStatus      != NO_ERROR ||
      PortError.MaxMessageSize != NO_ERROR ||
      PortError.MaxNBMessage   != NO_ERROR ||
      PortError.PortDirection  != NO_ERROR )
  {
     FlagOK = FALSE;

  }

  if(FlagOK == FALSE){
	  printf("5. Test failed\n");
  }



  /* 4.2 - Try to create a queuing port with a invalid MAX_MESSAGE_SIZE */
  if ( GetInvalidMessageSizeValue ( &InvalidMessageSizeValue ) == DEPENDENCE_AVAILABLE )
  {
     PortError =  CheckCreateQueuingPort( "QP_ERROR_HANDLING",
 	   			       InvalidMessageSizeValue, 
 		  		          QUEUING_PORT_MAX_MESSAGE_NB, 
			  	          SOURCE,
				          FIFO,
				          INVALID_CONFIG,
				          &PortId,
				          &PortStatus,
				          &RetCode );
    /* Evaluate errors */
    EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );
    if( PortError.ReturnCode     != NO_ERROR ||
        PortError.GetStatus      != NO_ERROR ||
        PortError.MaxMessageSize != NO_ERROR ||
        PortError.MaxNBMessage   != NO_ERROR ||
        PortError.PortDirection  != NO_ERROR )
     {
        FlagOK = FALSE;
     }

    if(FlagOK == FALSE){
  	  printf("6. Test failed\n");
    }

  }

  /* 4.3 - Try to create a queuing port with a above limit MAX_MESSAGE_SIZE */
  if ( GetAboveLimitMessageSizeValue ( &InvalidMessageSizeValue ) == DEPENDENCE_AVAILABLE )
  {
     PortError =  CheckCreateQueuingPort( "QP_ERROR_HANDLING",
 	   			       InvalidMessageSizeValue, 
 		  		          QUEUING_PORT_MAX_MESSAGE_NB, 
			  	          SOURCE,
				          FIFO,
				          INVALID_CONFIG,
				          &PortId,
				          &PortStatus,
				          &RetCode );
    /* Evaluate errors */
    EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );
    if( PortError.ReturnCode     != NO_ERROR ||
        PortError.GetStatus      != NO_ERROR ||
        PortError.MaxMessageSize != NO_ERROR ||
        PortError.MaxNBMessage   != NO_ERROR ||
        PortError.PortDirection  != NO_ERROR )
     {
        FlagOK = FALSE;

     }
    if(FlagOK == FALSE){
  	  printf("7. Test failed\n");
    }

  }

  /* 5.1 - Try to create a queuing port with a MAX_NB_MESSAGE */
  /*       value incompatible with configuration  */
  PortError = CheckCreateQueuingPort( "QP_ERROR_HANDLING",
 			         QUEUING_PORT_MAX_MESSAGE_SIZE, 
 				      QUEUING_PORT_MAX_MESSAGE_NB - 1, 
				      SOURCE,
				      FIFO,
				      INVALID_CONFIG,
				      &PortId,
				      &PortStatus,
				      &RetCode );
  /* Evaluate errors */
  EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );
  if( PortError.ReturnCode     != NO_ERROR ||
      PortError.GetStatus      != NO_ERROR ||
      PortError.MaxMessageSize != NO_ERROR ||
      PortError.MaxNBMessage   != NO_ERROR ||
      PortError.PortDirection  != NO_ERROR )
  {
     FlagOK = FALSE;

  }

  if(FlagOK == FALSE){
	  printf("8. Test failed\n");
  }


  /* 5.2 - Try to create a queuing port with a invalid MAX_NB_MESSAGE */
  if ( GetInvalidMessageRangeValue ( &InvalidMessageRangeValue ) == DEPENDENCE_AVAILABLE )
  {
     PortError = CheckCreateQueuingPort( "QP_ERROR_HANDLING",
 		   	         QUEUING_PORT_MAX_MESSAGE_SIZE, 
 			   	      InvalidMessageRangeValue, 
				         SOURCE,
				         FIFO,
				         INVALID_CONFIG,
				         &PortId,
				         &PortStatus,
				         &RetCode );
     /* Evaluate errors */
     EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );
     if( PortError.ReturnCode     != NO_ERROR ||
         PortError.GetStatus      != NO_ERROR ||
         PortError.MaxMessageSize != NO_ERROR ||
         PortError.MaxNBMessage   != NO_ERROR ||
         PortError.PortDirection  != NO_ERROR )
     {
        FlagOK = FALSE;
     }

     if(FlagOK == FALSE){
    	 printf("9. Test failed\n");
     }

  }
  
  /* 5.3 - Try to create a queuing port with a above limit MAX_NB_MESSAGE */
  if ( GetAboveLimitMessageRangeValue ( &InvalidMessageRangeValue ) == DEPENDENCE_AVAILABLE )
  {
     PortError = CheckCreateQueuingPort( "QP_ERROR_HANDLING",
 		   	         QUEUING_PORT_MAX_MESSAGE_SIZE, 
 			   	      InvalidMessageRangeValue, 
				         SOURCE,
				         FIFO,
				         INVALID_CONFIG,
				         &PortId,
				         &PortStatus,
				         &RetCode );
     /* Evaluate errors */
     EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );
     if( PortError.ReturnCode     != NO_ERROR ||
         PortError.GetStatus      != NO_ERROR ||
         PortError.MaxMessageSize != NO_ERROR ||
         PortError.MaxNBMessage   != NO_ERROR ||
         PortError.PortDirection  != NO_ERROR )
     {
        FlagOK = FALSE;

     }
     if(FlagOK == FALSE){
   	  printf("11. Test failed\n");
     }

  }
  

  /*testing*/
  *TestResult = EvaluateResult( FlagOK );
  return;

  /* 6 - Try create a QUEUING port, exceeding the */
  /*     MAX_NUMBER_OF_QUEUING_PORTS */

  if ( DoQueuingPortExaustion ( &FlagOK ) == DEPENDENCE_AVAILABLE ){
	  printf("calling CheckCreateQueuingPort\n");

     PortError = CheckCreateQueuingPort( "QP_OUT_OF_RANGE",
                                         QUEUING_PORT_MAX_MESSAGE_SIZE, 
 				                         QUEUING_PORT_MAX_MESSAGE_NB, 
			                             SOURCE,
			                             FIFO,
			                             INVALID_CONFIG,
			                             &PortId,
			                             &PortStatus,
                                         &RetCode );

   /* Evaluate errors */
     EvaluateErrorType( RetCode, &FlagOK, INVALID_CONFIG );

     if(FlagOK == FALSE){
    	 printf("12: PortError.ReturnCode: %d\n", PortError.ReturnCode);
    	 printf("12: PortError.GetStatus: %d\n", PortError.GetStatus);
    	 printf("12: PortError.MaxMessageSize: %d\n", PortError.MaxMessageSize);
    	 printf("12: PortError.MaxNBMessage: %d\n", PortError.MaxNBMessage);
    	 printf("12: PortError.PortDirection: %d\n",PortError.PortDirection);
     }


     if ( PortError.ReturnCode     != NO_ERROR ||
          PortError.GetStatus      != NO_ERROR ||
          PortError.MaxMessageSize != NO_ERROR ||
          PortError.MaxNBMessage   != NO_ERROR ||
          PortError.PortDirection  != NO_ERROR )
     {
        FlagOK = FALSE;
     }
     if(FlagOK == FALSE){
   	  printf("12. Test failed\n");
     }

  }
    
  /* Fill return value */
  *TestResult = EvaluateResult( FlagOK );
  
} /* Test_T_API_INTER_570_0012 */



/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |               END OF FILE: Test_INTER_570_0012.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
