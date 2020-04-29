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
 * | FILE: AVT_SamplingPorts.c                                          | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: SAMPLINGPORTS                                           | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): CheckCreateSamplingPort                     | *
 * |                        CheckWriteSamplingMessage                   | *
 * |                        CheckReadSamplingMessage                    | *
 * |                        CheckGetSamplingPortId                      | *
 * |                        CheckGetSamplingPortStatus                  | *
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
 * | DATE:    29/08/2002                                                | *
 * |                                                                    | *
 * | FTP REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "AVT_SamplingPorts.h"


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckCtrateSamplingPort.c                           | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/08/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | SAMPLING_PORT_NAME_TYPE                                            | *
 * |     InPortName                  X            Name given to the     | *
 * |                                              sampling port (SP).   | *
 * | MESSAGE_SIZE_TYPE                                                  | *
 * |     InMaxMessageSize            X            Size given to the SP. | *
 * | PORT_DIRECTION_TYPE                                                | *
 * |     InPortDirection             X            Direction(src/dest)   | *
 * |                                              given to the QP.      | *
 * | SYSTEM_TIME_TYPE                                                   | *
 * |     InRefreshPeriod             X            Refreshing period.    | *
 * | RETURN_CODE_TYPE InReturnCode   X            Return code that we   | *
 * |                                              espect to obtain after| *
 * |                                              call the function.    | *
 * | SAMPLING_PORT_ID_TYPE  *OutPortId       X    Returned SP id        | *
 * |                                              resulting from the    | *
 * |                                              call of the function  | *
 * |                                              under test.           | *
 * | SAMPLING_PORT_STATUS_TYPE *OutSatus     X    Structure filled with | *
 * |                                              information regarding | *
 * |                                              the status of the     | *
 * |                                              operation.            | *
 * | RETUEN_CODE_TYPE *OutReturnCode         X    Return code of the    | *
 * |                                              ARINC function that   | *
 * |                                              was called.           | *
 * | RETURN VALUE:                                                      | *
 * | CREATE_SAMPLING_PORT_ERROR                   Bit structure holding | *
 * |                                              the different types of| *
 * |                                              errors that can occur | *
 * |                                              where the error that  | *
 * |                                              occurred has its bit  | *
 * |                                              set to one. The others! *
 * |                                              are set to 0.         | *
 * ---------------------------------------------------------------------- */
CREATE_SAMPLING_PORT_ERROR
CheckCreateSamplingPort( SAMPLING_PORT_NAME_TYPE    InPortName,
                         MESSAGE_SIZE_TYPE          InMaxMessageSize,
                         PORT_DIRECTION_TYPE        InPortDirection,
                         SYSTEM_TIME_TYPE           InRefreshPeriod,
                         RETURN_CODE_TYPE           InReturnCode,
                         SAMPLING_PORT_ID_TYPE     *OutPortId,
                         SAMPLING_PORT_STATUS_TYPE *OutStatus,
                         RETURN_CODE_TYPE          *OutReturnCode )
{
  char *TabPortDir[2] = { "SOURCE", "DESTINATION" };
  CREATE_SAMPLING_PORT_ERROR ReturnError;
  RETURN_CODE_TYPE           ReturnCode = NO_ERROR;

  printf("CheckCreateSamplingPort %s\n", InPortName);

  /* Clean error struct */
  ReturnError.ReturnCode     = NO_ERROR;
  ReturnError.GetStatus      = NO_ERROR;
  ReturnError.MaxMessageSize = NO_ERROR;
  ReturnError.PortDirection  = NO_ERROR;
  ReturnError.RefreshPeriod  = NO_ERROR;

  /* Clean all output parameters */
  memset( (void*)OutStatus, '\0', sizeof(SAMPLING_PORT_STATUS_TYPE));
  *OutReturnCode = NO_ERROR;
  *OutPortId     = 0;

  /* Call ARINC653 function */
  w_CREATE_SAMPLING_PORT( InPortName,
			  InMaxMessageSize,
			  InPortDirection,
			  InRefreshPeriod,
			  OutPortId,
			  OutReturnCode );

  /* Evaluate the results */

  /* RETURN CODE */
  if( *OutReturnCode != InReturnCode )
  {
    ReturnError.ReturnCode = AVT_ERROR;
    WriteLogReturnCode("CheckCreateSamplingPort", 
		       InReturnCode, 
		       *OutReturnCode );
  }

  if( *OutReturnCode == NO_ERROR )
  {
    w_GET_SAMPLING_PORT_STATUS( *OutPortId, OutStatus, &ReturnCode );


    if( ReturnCode == NO_ERROR )
    {


      /* MAX MESSAGE SIZE */
      if( OutStatus->MAX_MESSAGE_SIZE != InMaxMessageSize )
      {
        ReturnError.MaxMessageSize = AVT_ERROR;
	WriteLogErrorNum( "CheckCreateSamplingPort",
			  "MAX_MESSAGE_SIZE",
			  InMaxMessageSize,
			  OutStatus->MAX_MESSAGE_SIZE );
      }

      /* PORT DIRECTION */
      if( OutStatus->PORT_DIRECTION != InPortDirection )
      {
        ReturnError.PortDirection = AVT_ERROR;
	WriteLogErrorStr( "CheckCreateSamplingPort",
			  "PORT_DIRECTION",
			  TabPortDir[InPortDirection],
			  TabPortDir[OutStatus->PORT_DIRECTION] );
      }

      /* REFRESH PERIOD */
      if( OutStatus->REFRESH_PERIOD != InRefreshPeriod )
      {
        ReturnError.RefreshPeriod = AVT_ERROR;
	WriteLogErrorNum( "CheckCreateSamplingPort",
			  "REFRESH_PERIOD",
			  InRefreshPeriod,
			  OutStatus->REFRESH_PERIOD );
      }
    }
    else
    {

      ReturnError.GetStatus = AVT_ERROR;
      WriteLogErrorFunc( ReturnCode, 
			 "CheckCreateSamplingPort", 
			 "GET_SAMPLING_PORT_STATUS" );   
    }
  }
  return ReturnError;

} /* CheckCreateSamplingPort */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckWriteSamplingMessage                           | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/08/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | SAMPLING_PORT_ID_TYPE InPortId  X            SP identification.    | *
 * | APEX_BYTE          *InMessage   X            Message to be sent    | *
 * |                                              through the SP.       | *
 * | MESSAGE_SIZE_TYPE  InSize       X            Size of the message   | *
 * |                                              to be sen through SP. | *
 * | RETURN_CODE_TYPE InReturnCode   X            Return code that we   | *
 * |                                              espect to obtain after| *
 * |                                              call the function.    | *
 * | SAMPLING_PORT_STATUS_TYPE *OutSatus     X    Structure filled with | *
 * |                                              information regarding | *
 * |                                              the status of the     | *
 * |                                              operation.            | *
 * | RETUEN_CODE_TYPE *OutReturnCode         X    Return code of the    | *
 * |                                              ARINC function that   | *
 * |                                              was called.           | *
 * | RETURN VALUE:                                                      | *
 * | WRITE_SAMPLING_MESSAGE_ERROR                 Bit structure holding | *
 * |                                              the different types of| *
 * |                                              errors that can occur | *
 * |                                              where the error that  | *
 * |                                              occurred has its bit  | *
 * |                                              set to one. The others! *
 * |                                              are set to 0.         | *
 * ---------------------------------------------------------------------- */
WRITE_SAMPLING_MESSAGE_ERROR
CheckWriteSamplingMessage( SAMPLING_PORT_ID_TYPE      InPortId,
                           APEX_BYTE                 *InMessage,
                           MESSAGE_SIZE_TYPE          InSize,
                           RETURN_CODE_TYPE           InReturnCode,
                           SAMPLING_PORT_STATUS_TYPE *OutStatus,
                           RETURN_CODE_TYPE          *OutReturnCode )
{
  WRITE_SAMPLING_MESSAGE_ERROR ReturnError;
  RETURN_CODE_TYPE             ReturnCode = NO_ERROR;

  printf("CheckWriteSamplingMessage %d\n", InPortId);

  /* Clean error struct */
  ReturnError.ReturnCode = NO_ERROR;
  ReturnError.GetStatus  = NO_ERROR;

  /* Clean all output parameters */
  memset( (void*)OutStatus, '\0', sizeof(SAMPLING_PORT_STATUS_TYPE));
  *OutReturnCode = NO_ERROR;

  /* Call ARINC653 function */
  w_WRITE_SAMPLING_MESSAGE( InPortId, InMessage, InSize, OutReturnCode );

  /* Evaluate the results */

  /* RETURN CODE */
  if( *OutReturnCode != InReturnCode )
  {


    ReturnError.ReturnCode = AVT_ERROR;
    WriteLogReturnCode("CheckWriteSamplingMessage", 
		       InReturnCode, 
		       *OutReturnCode );
  }

  /* If WRITE_SAMPLING_MESSAGE returns NO_ERROR, execute  */
  /* GET_SAMPLING_PORT_STATUS  and return the status  (merely informative) */
  if( *OutReturnCode == NO_ERROR )
  {
    w_GET_SAMPLING_PORT_STATUS( InPortId, OutStatus, &ReturnCode );
    if( ReturnCode != NO_ERROR )
    {
      ReturnError.GetStatus = AVT_ERROR;
      WriteLogErrorFunc( ReturnCode, 
			 "CheckWriteSamplingMessage", 
			 "GET_SAMPLING_PORT_STATUS" );  
    }
  }
  return ReturnError;

} /* CheckWriteSamplingMessage */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckReadSamplingMessage                            | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/08/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | SAMPLING_PORT_ID_TYPE InPortId  X            SP identification.    | *
 * | RETURN_CODE_TYPE InReturnCode   X            Expected return code, | *
 * |                                              resulting from the    | *
 * |                                              call of the function  | *
 * |                                              under test.           | *
 * | APEX_BYTE         *InMessage    X            Expected received msg.| *
 * | MESSAGE_SIZE_TYPE  InMsgSize    X            Expected message size | *
 * | APEX_BYTE         *OutMessage           X    Message received      | *
 * |                                              through the QP.       | *
 * | MESSAGE_SIZE_TYPE *OutMsgSize           X    Size of the received  | *
 * |                                              message.              | *
 * | VALIDITY_TYPE     *OutPortValidity      X    Validity of the SP    | *
 * |                                              message.              | *
 * | SAMPLING_PORT_STATUS_TYPE *OutSatus     X    Structure filled with | *
 * |                                              information regarding | *
 * |                                              the status of the     | *
 * |                                              operation.            | *
 * | RETUEN_CODE_TYPE *OutReturnCode         X    Return code of the    | *
 * |                                              ARINC function that   | *
 * |                                              was called.           | *
 * | RETURN VALUE:                                                      | *
 * | READ_SAMPLING_PORT_ERROR                     Bit structure holding | *
 * |                                              the different types of| *
 * |                                              errors that can occur | *
 * |                                              where the error that  | *
 * |                                              occurred has its bit  | *
 * |                                              set to one. The others! *
 * |                                              are set to 0.         | *
 * ---------------------------------------------------------------------- */
READ_SAMPLING_MESSAGE_ERROR
CheckReadSamplingMessage( SAMPLING_PORT_ID_TYPE      InPortId,
			  RETURN_CODE_TYPE           InReturnCode,
			  APEX_BYTE                 *InMessage,
			  MESSAGE_SIZE_TYPE          InMsgSize,
			  APEX_BYTE                 *OutMessage,
			  MESSAGE_SIZE_TYPE         *OutMsgSize,
			  VALIDITY_TYPE             *OutPortValidity,
			  SAMPLING_PORT_STATUS_TYPE *OutStatus,
			  RETURN_CODE_TYPE          *OutReturnCode,
			  int                       LogFlag )

{

	printf("CheckReadSamplingMessage %d\n", InPortId);
  READ_SAMPLING_MESSAGE_ERROR ReturnError;
  RETURN_CODE_TYPE            ReturnCode = NO_ERROR;
  /* Clean error struct */
  ReturnError.ReturnCode   = NO_ERROR;
  ReturnError.MessageSize  = NO_ERROR;
  ReturnError.Message      = NO_ERROR;
  ReturnError.GetStatus    = NO_ERROR;

  /* Clean all output parameters */
  memset( (void*)OutStatus, '\0', sizeof ( SAMPLING_PORT_STATUS_TYPE ) );
  *OutReturnCode = NO_ERROR;

  /* Call ARINC653 function */
  w_READ_SAMPLING_MESSAGE( InPortId,
			   OutMessage,
			   OutMsgSize,
			   OutPortValidity,
			   OutReturnCode);

  /* Evaluate the results */

  /* RETURN CODE */
  if( *OutReturnCode != InReturnCode ){
	  printf("*OutReturnCode != InReturnCode \n");
	  printf("expected %d and received %d\n", InReturnCode, *OutReturnCode);
    ReturnError.ReturnCode = AVT_ERROR;
    if( LogFlag == LOG_ERROR ){
      WriteLogReturnCode("CheckReadSamplingMessage", 
			 InReturnCode,
			 *OutReturnCode );
    }
  }

  /* MESSAGE SIZE */
  if( *OutMsgSize != InMsgSize ){
	  printf("*OutMsgSize != InMsgSize \n");
	  printf("expected %d and received %d\n", InReturnCode, *OutReturnCode);
    ReturnError.MessageSize = AVT_ERROR;
    if( LogFlag == LOG_ERROR )
    {
      WriteLogErrorNum( "CheckReadSamplingMessage",
			"MESSAGE_SIZE",
			InMsgSize,
			*OutMsgSize );
    }
  }else
	  /* MESSAGE */



	  if( strncmp( OutMessage, InMessage, *OutMsgSize) != 0){
		  ReturnError.Message = AVT_ERROR;
		  if( LogFlag == LOG_ERROR ) {
			  WriteLogErrorStr( "CheckReadSamplingMessage",
					  "MESSAGE",
					  InMessage,
					  OutMessage );
		  }
	  }


  /* If READ_SAMPLING_MESSAGE returns NO_ERROR, execute  */
  /* GET_SAMPLING_PORT_STATUS  and return the status  (merely informative) */
  if( *OutReturnCode == NO_ERROR ){
	  //printf("*OutReturnCode == NO_ERROR \n");
	w_GET_SAMPLING_PORT_STATUS( InPortId, OutStatus, &ReturnCode );

    if( ReturnCode != NO_ERROR ){
    	//printf("ReturnCode != NO_ERROR\n");

      ReturnError.GetStatus = AVT_ERROR;
      if( LogFlag == LOG_ERROR )
      {
	WriteLogErrorFunc( ReturnCode, 
			   "CheckReadSamplingMessage", 
			   "GET_SAMPLING_PORT_STATUS" );   
      }
    }//else printf("w_GET_SAMPLING_PORT_STATUS NO_ERROR\n");

  }

//printf("Leaving CheckReadSamplingMessage\n");

  return ReturnError;

} /* CheckReadSamplingMessage */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckGetSamplingPortId                              | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/08/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | SAMPLING_PORT_NAME_TYPE                                            | *
 * |                 InPortName      X            Name given to SP.     | *
 * | RETURN_CODE_TYPE InReturnCode   X            Expected return code, | *
 * |                                              resulting from the    | *
 * |                                              call of the function  | *
 * |                                              under test.           | *
 * |  SAMPLING_PORT_ID_TYPE *OutPortId       X    Identification given  | *
 * |                                              to SP.                | *
 * | SAMPLING_PORT_STATUS_TYPE *OutSatus     X    Structure filled with | *
 * |                                              information regarding | *
 * |                                              the status of the     | *
 * |                                              operation.            | *
 * | RETUEN_CODE_TYPE *OutReturnCode         X    Return code of the    | *
 * |                                              ARINC function that   | *
 * |                                              was called.           | *
 * | RETURN VALUE:                                                      | *
 * | GET_SAMPLING_PORT_ID_ERROR                   Bit structure holding | *
 * |                                              the different types of| *
 * |                                              errors that can occur | *
 * |                                              where the error that  | *
 * |                                              occurred has its bit  | *
 * |                                              set to one. The others! *
 * |                                              are set to 0.         | *
 * ---------------------------------------------------------------------- */
GET_SAMPLING_PORT_ID_ERROR
CheckGetSamplingPortId(  SAMPLING_PORT_NAME_TYPE    InPortName,
                         RETURN_CODE_TYPE           InReturnCode,
                         SAMPLING_PORT_ID_TYPE     *OutPortId,
                         SAMPLING_PORT_STATUS_TYPE *OutStatus,
                         RETURN_CODE_TYPE          *OutReturnCode )
{
  RETURN_CODE_TYPE           ReturnCode;
  GET_SAMPLING_PORT_ID_ERROR ReturnError;

  /* Clean error struct */
  ReturnError.ReturnCode = NO_ERROR;
  ReturnError.GetStatus  = NO_ERROR;

  /* Clean all output parameters */
  memset( (void*)OutStatus, '\0', sizeof(SAMPLING_PORT_STATUS_TYPE));
  *OutReturnCode = NO_ERROR;
  *OutPortId     = 0;

  /* Call ARINC653 function */
  w_GET_SAMPLING_PORT_ID( InPortName, OutPortId, OutReturnCode );

  /* Evaluate the results */

  /* RETURN CODE */
  if( *OutReturnCode != InReturnCode )
  {
    ReturnError.ReturnCode = AVT_ERROR;
    WriteLogReturnCode("CheckGetSamplingPortId", 
		       InReturnCode, 
		       *OutReturnCode );
  }

  /* If NO_ERROR is returned */
  if( *OutReturnCode == NO_ERROR )
  {
    /* Returns NO_ERROR, execute  */
    /* GET_SAMPLING_PORT_STATUS  and return the status  (merely informative) */
    w_GET_SAMPLING_PORT_STATUS( *OutPortId, OutStatus, &ReturnCode );
    if( ReturnCode != NO_ERROR )
    {
      ReturnError.GetStatus = AVT_ERROR;
      WriteLogErrorFunc( ReturnCode, 
			 "CheckGetSamplingPortId", 
			 "GET_SAMPLING_PORT_STATUS" );
    }
  }
  return ReturnError;

} /* CheckGetSamplingPortId */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckGetSamplingPortStatus                          | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/08/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | SAMPLING_PORT_ID_TYPE InPortId  X            SP identification.    | *
 * | SAMPLING_PORT_STATUS_TYPE *OutSatus     X    Structure filled with | *
 * |                                              information regarding | *
 * |                                              the status of the     | *
 * |                                              operation.            | *
 * | RETUEN_CODE_TYPE *OutReturnCode         X    Return code of the    | *
 * |                                              ARINC function that   | *
 * |                                              was called.           | *
 * | RETURN VALUE:                                                      | *
 * | GET_SAMPLING_PORT_STATUS_ERROR               Bit structure holding | *
 * |                                              the different types of| *
 * |                                              errors that can occur | *
 * |                                              where the error that  | *
 * |                                              occurred has its bit  | *
 * |                                              set to one. The others! *
 * |                                              are set to 0.         | *
 * ---------------------------------------------------------------------- */
GET_SAMPLING_PORT_STATUS_ERROR
CheckGetSamplingPortStatus( SAMPLING_PORT_ID_TYPE      InPortId,
                            RETURN_CODE_TYPE           InReturnCode,
                            SAMPLING_PORT_STATUS_TYPE *OutStatus,
                            RETURN_CODE_TYPE          *OutReturnCode )
{
  GET_SAMPLING_PORT_STATUS_ERROR ReturnError;

  /* Clean error struct */
  ReturnError.ReturnCode = NO_ERROR;

  /* Clean all output parameters */
  memset( (void*)OutStatus, '\0', sizeof(SAMPLING_PORT_STATUS_TYPE) );
  *OutReturnCode = NO_ERROR;

  /* Call ARINC653 function */
  w_GET_SAMPLING_PORT_STATUS( InPortId, OutStatus, OutReturnCode );

  if( *OutReturnCode != InReturnCode )
  {
     ReturnError.ReturnCode = AVT_ERROR;
     WriteLogReturnCode("CheckGetSamplingPortStatus", 
			InReturnCode,
			*OutReturnCode );
  }
  return ReturnError;

} /* CheckGetSamplingPortStatus */


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                  END OF FILE: AVT_SamplingPorts.c                  | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
