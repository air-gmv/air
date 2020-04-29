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
 * | FILE: AVT_Funcs.c                                                  | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVTFUNCS                                                | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S):  SetEvent                                   | *
 * |                         WaitEvent                                  | *
 * |                         ResetEvent                                 | *
 * |                         GetEventStatus                             | *
 * |                         ReadBlackboard                             | *
 * |                         DisplayBlackboard                          | *
 * |                         WriteSamplingMessage                       | *
 * |                         ReadSamplingMessage                        | *
 * |                         SendQueuingMessage                         | *
 * |                         ReceiveQueuingMessage                      | *
 * |                         CleanQueuingPort                           | *
 * |                         CleanBuffer                                | *
 * |                         CleanBlackboard                            | *
 * |                         StartStopProcess                           | *
 * |                         SuspendProcess                             | *
 * |                         Resume                                     | *
 * |                         UnlockPreemption                           | *
 * |                         SetPriority                                | *
 * |                         EvaluateResult                             | *
 * |                         EvaluateProcessState                       | *
 * |                         EvaluateMessage                            | *
 * |                         PutOkFailInMessage                         | *
 * |                         SignalSemaphore                            | *
 * |                         WaitSemaphore                              | *
 * |                         GetProcessStatus                           | *
 * |                         DisplayProcessStatus                       | *
 * |                         EvaluateError                              | *
 * |                         EvaluateErrorType                          | *
 * |                         WriteLogReturnCode                         | *
 * |                                                                    | *
 * | DESCRIPTION: Auxiliary functions                                   | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    25/09/2002...                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "AVT_Funcs.h"
#include <string.h>

extern int NrTestId;
extern int TraceActive;





/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: WriteSamplingMessage                                | *
 * |                                                                    | *
 * | DESCRIPTION: Make WRITE_SAMPLING_MESSAGE by name                   | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   18/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | SAMPLING_PORT_NAME_TYPE *SPName X                                  | *
 * | MESSAGE_ADDR_TYPE     *Message  X                                  | *
 * | MESSAGE_SIZE_TYPE      Length   X                                  | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE
WriteSamplingMessage( SAMPLING_PORT_NAME_TYPE SPName,
          MESSAGE_ADDR_TYPE       Message,
          MESSAGE_SIZE_TYPE       Length )
{
  RETURN_CODE_TYPE      RetCode = NO_ERROR;
  SAMPLING_PORT_ID_TYPE PortId  = 0;

  printf("WriteSamplingMessage %s msg: %s\n", SPName, Message);

  w_GET_SAMPLING_PORT_ID( SPName, &PortId, &RetCode );
  if( RetCode == NO_ERROR )
  {
    w_WRITE_SAMPLING_MESSAGE( PortId, Message, Length, &RetCode );
    if( RetCode != NO_ERROR )
    {
      WriteLogErrorFunc( RetCode,
       "WriteSamplingMessage",
       "WRITE_SAMPLING_MESSAGE" );
    }
  }
  else
  {
    WriteLogErrorFunc( RetCode,
           "WriteSamplingMessage",
           "GET_SAMPLING_PORT_ID" );
  }
  return RetCode;

} /* WriteSamplingMessage */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:ReadSamplingMessage                                  | *
 * |                                                                    | *
 * | DESCRIPTION: Make READ_SAMPLING_MESSAGE by name                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   18/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | SAMPLING_PORT_NAME_TYPE *SPName X                                  | *
 * | MESSAGE_ADDR_TYPE       *Messag        X                           | *
 * | BOOLEAN                 *Validity      X                           | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE
ReadSamplingMessage( SAMPLING_PORT_NAME_TYPE  SPName,
         MESSAGE_ADDR_TYPE        Message,
         VALIDITY_TYPE            *Validity )
{
  RETURN_CODE_TYPE      RetCode = NO_ERROR;
  MESSAGE_SIZE_TYPE     Length  = 0;
  SAMPLING_PORT_ID_TYPE PortId  = 0;
  APEX_BYTE             wMessage[1024];

  w_GET_SAMPLING_PORT_ID( SPName, &PortId, &RetCode );
  if( RetCode == NO_ERROR )
  {
    memset( wMessage, '\0', sizeof( wMessage ));
    w_READ_SAMPLING_MESSAGE( PortId, wMessage, &Length, Validity, &RetCode );
    //strncpy( Message, wMessage, Length );
    memcpy( Message, wMessage, Length );
    Message[Length] = '\0';
    if( RetCode != NO_ERROR )
    {
      WriteLogErrorFunc( RetCode,
       "ReadSamplingMessage",
       "READ_SAMPLING_MESSAGE" );
    }
  }
  else
  {
    WriteLogErrorFunc( RetCode,
           "ReadSamplingMessage",
           "GET_SAMPLING_PORT_ID" );
  }
  return RetCode;

} /* ReadSamplingMessage */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: SendQueuingMessage                                  | *
 * |                                                                    | *
 * | DESCRIPTION: Make SEND_QUEUING_MESSAGE by name                     | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   18/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | QUEUING_PORT_NAME_TYPE *QPName  X                                  | *
 * | MESSAGE_ADDR_TYPE      *Message X                                  | *
 * | MESSAGE_SIZE_TYPE       Length  X                                  | *
 * | SYSTEM_TIME_TYPE        TimeOut X                                  | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE
SendQueuingMessage ( QUEUING_PORT_NAME_TYPE QPName,
         MESSAGE_ADDR_TYPE      Message,
         MESSAGE_SIZE_TYPE      Length,
         SYSTEM_TIME_TYPE       TimeOut,
         int                    LogFlag)
{
  RETURN_CODE_TYPE      RetCode = NO_ERROR;
  QUEUING_PORT_ID_TYPE  PortId  = 0;

  if(Message == NULL) printf("input msg is null\n");

  printf("SendQueuingMessage %s, msg: %s\n", QPName, Message);

  w_GET_QUEUING_PORT_ID( QPName, &PortId, &RetCode );
  if( RetCode == NO_ERROR )
  {
    w_SEND_QUEUING_MESSAGE( PortId, Message, Length, TimeOut, &RetCode );
    if( RetCode != NO_ERROR && LogFlag == LOG_ERROR )
    {
      WriteLogErrorFunc( RetCode,
       "SendQueuingMessage",
       "SEND_QUEUING_MESSAGE" );
    }
  }
  else
  {
    WriteLogErrorFunc( RetCode,
           "SendQueuingMessage",
           "GET_QUEUING_PORT_ID" );
  }
  return RetCode;

} /* SendQueuingMessage */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: ReceiveQueuingMessage                               | *
 * |                                                                    | *
 * | DESCRIPTION: Make RECEIVE_QUEUING_MESSAGE by name                  | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   18/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | QUEUING_PORT_NAME_TYPE *QPNam   X                                  | *
 * | SYSTEM_TIME_TYPE      TimeOut   X                                  | *
 * | MESSAGE_SIZE_TYPE      MaxLen   X                                  | *
 * | MESSAGE_ADDR_TYPE    *Message          X                           | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE
ReceiveQueuingMessage ( QUEUING_PORT_NAME_TYPE QPName,
      SYSTEM_TIME_TYPE       TimeOut,
      MESSAGE_ADDR_TYPE      Message )
{
  RETURN_CODE_TYPE      RetCode = NO_ERROR;
  QUEUING_PORT_ID_TYPE  PortId  = 0;
  MESSAGE_SIZE_TYPE     Length  = 0;
  APEX_BYTE             wMessage[64];

  printf("ReceiveQueuingMessage name:%s timeout:%d\n", QPName, TimeOut);

  printf("w_GET_QUEUING_PORT_ID\n");
  w_GET_QUEUING_PORT_ID( QPName, &PortId, &RetCode );
  if( RetCode == NO_ERROR )
  {
    memset( wMessage, '\0', sizeof( wMessage ));

    w_RECEIVE_QUEUING_MESSAGE( PortId,
             TimeOut,
             wMessage,
             &Length,
             &RetCode );

	 if ( Length > 0 )
	 {
       memcpy( Message, wMessage, Length );
       printf("wMessage: %s\n", wMessage);
	 }
	 
	 if( Length < 64 )
    {
       Message[Length] = '\0';
    }

    if( RetCode != NO_ERROR )
    {
    	printf("w_RECEIVE_QUEUING_MESSAGE failed %d\n", RetCode);
      WriteLogErrorFunc( RetCode,
       "ReceiveQueuingMessage",
       "RECEIVE_QUEUING_MESSAGE" );
    }
  }
  else
  {
	  printf("w_GET_QUEUING_PORT_ID failed: %d\n", RetCode);
    WriteLogErrorFunc( RetCode,
           "ReceiveQueuingMessage",
           "GET_QUEUING_PORT_ID" );
  }
  return RetCode;

} /* ReceiveQueuingMessage */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CleanFullQueuingPort                                | *
 * |                                                                    | *
 * | DESCRIPTION: Retrieves all messages from a full queuing port       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   01/08/2007                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE CleanFullQueuingPort ( QUEUING_PORT_NAME_TYPE QPName )
{
	RETURN_CODE_TYPE            RetCode = NO_ERROR;
	BOOLEAN                     FlagOK  = TRUE;
	APEX_BYTE                   Message[QUEUING_PORT_MAX_MESSAGE_SIZE];
	int                         i = 0;

	for (i = 0; i < QUEING_PORT_MAX_MESSAGE_NB_BOTH_ENDS; i ++)
	{
	   memset( Message, '\0', sizeof(Message));
	   RetCode = ReceiveQueuingMessage ( QPName, INFINITE_TIME_VALUE, Message );
	   EvaluateError( RetCode, &FlagOK );
	}
	
	return RetCode;
	
} /* CleanFullQueuingPort */ 

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CleanQueuingPort                                    | *
 * |                                                                    | *
 * | DESCRIPTION: Make several readings to port in order to read        | *
 * |              possible pending messages.                            | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   18/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | QUEUING_PORT_NAME_TYPE *QPName  X            Port name             | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE
CleanQueuingPort ( QUEUING_PORT_NAME_TYPE QPName )
{
  RETURN_CODE_TYPE         RetCode = NO_ERROR;
  QUEUING_PORT_ID_TYPE     PortId  = 0;
  MESSAGE_SIZE_TYPE        Length  = 0;
  APEX_BYTE                Message[64];
  QUEUING_PORT_STATUS_TYPE PortStatus;

 // printf("CleanQueuingPort %s\n", QPName);

  w_GET_QUEUING_PORT_ID( QPName, &PortId, &RetCode );
  if( RetCode == NO_ERROR )
  {
     w_GET_QUEUING_PORT_STATUS( PortId, &PortStatus, &RetCode );
  }

  if( RetCode == NO_ERROR )
  {
    while ( PortStatus.NB_MESSAGE > 0 )
    {
      memset( Message, '\0', sizeof(Message ));
      RECEIVE_QUEUING_MESSAGE( PortId, 0, Message, &Length, &RetCode );
      w_GET_QUEUING_PORT_STATUS( PortId, &PortStatus, &RetCode );
    }
  }
  return RetCode;

} /* CleanQueuingPort */




void test_id(int i_id, char * o_id){

	printf("test_id: %d\n", i_id);
	switch(i_id){
		case T_API_INTER_580_0010:
			strcpy(o_id, "6");
			break;
		case T_API_INTER_590_0010:
			strcpy(o_id, "27");
			break;

		default:
			strcpy(o_id, "42");

	}

}


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: SendSubtestIdToAuxPartition                         | *
 * |                                                                    | *
 * | DESCRIPTION: Send by QueuingPort, de subtest Id from MasterTest    | *
 * |              to AuxPartition.                                      | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   18/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | int SubtestId                   X            Sub test Id           | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE
SendSubtestIdToAuxPartition( int SubtestId )
{
  RETURN_CODE_TYPE RetCode = NO_ERROR;
  APEX_BYTE        Message[QUEUING_PORT_MAX_MESSAGE_SIZE];
  char id[3];
  memset(id, 0, 3);

  memset( Message, '\0', sizeof( Message ));
  //sprintf( Message, "%d",  SubtestId );

  test_id(SubtestId, id);
  printf("SendSubtestIdToAuxPartition %s\n", id);

  strcat((char *)Message, id);
  RetCode = SendQueuingMessage ( "QP_SUBTESTID_SEND",
                                 Message,
                                 strlen( Message ),
                                 INFINITE_TIME_VALUE,
                                  LOG_ERROR);

  if( RetCode != NO_ERROR )
  {
    WriteLogErrorFunc( RetCode,
           "SendSubtestIdToAuxPartition", "SendQueuingMessage" );
  }
  return RetCode;

} /* SendSubtestIdToAuxPartition */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: ReadResultsFromAuxPartition                         | *
 * |                                                                    | *
 * | DESCRIPTION: Receive by queuing port the result of the procedure   | *
 * |              made in auxiliary partition.                          | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   18/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | BOOLEAN *Result                        X     Result of AuxPartition| *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE
ReadResultsFromAuxPartition( BOOLEAN *Result )
{
  RETURN_CODE_TYPE     RetCode = NO_ERROR;
  QUEUING_PORT_ID_TYPE PortId  = 0;
  MESSAGE_SIZE_TYPE    Length  = 0;
  APEX_BYTE            Message[256];
  /* Get port id */

  printf("ReadResultsFromAuxPartition\n");
  *Result = FALSE;
   memset (Message, 0, 256);

  w_GET_QUEUING_PORT_ID( "QP_RESULT_RECEIVE", &PortId, &RetCode );

  if( RetCode == NO_ERROR )
  {

    w_RECEIVE_QUEUING_MESSAGE ( PortId,
            INFINITE_TIME_VALUE,
            Message,
            &Length,
            &RetCode );


printf("Message: %s - %d\n", Message, Length);
    if ( RetCode == NO_ERROR )
    {
      if( strcmp (Message, "OK") == 0 )
      {
         *Result = TRUE;
      }
    }
    else
    {
      WriteLogErrorFunc( RetCode,
      "ReadResultsFromAuxPartition", "RECEIVE_QUEUING_MESSAGE" );
    }
  }
  else
  {
    WriteLogErrorFunc( RetCode,
           "ReadResultsFromAuxPartition", "GET_QUEUING_PORT_ID" );
  }
  return RetCode;

} /* ReadResultsFromAuxPartition */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: WriteResultsFromAuxPartition                        | *
 * |                                                                    | *
 * | DESCRIPTION: Send by queuing port the result of the procedure      | *
 * |              made in auxiliary partition.                          | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   29/11/2004                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | BOOLEAN *Result                 X            Result of AuxPartition| *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE
WriteResultsFromAuxPartition( BOOLEAN Result )
{
  RETURN_CODE_TYPE     RetCode = NO_ERROR;
  QUEUING_PORT_ID_TYPE PortId  = 0;
  APEX_BYTE            Message[256];
  int len = 0;
  printf("WriteResultsFromAuxPartition\n");

  unsigned char msg[10];
  memset(msg, 0, 10);

  /* Get port id */
  w_GET_QUEUING_PORT_ID( "QP_RESULT_SEND", &PortId, &RetCode );
  if( RetCode == NO_ERROR )
  {


    /* compose message to replay */
    if( Result == TRUE )
    {
      strcpy( msg, "OK" );
      len = 2;
    	//memcpy(Message, "OK", 2);
    }
    else
    {

      strcpy( msg, "Fail" );
      len = 4;
      //memcpy( Message, "Fail", 4 );
    }



    w_SEND_QUEUING_MESSAGE ( PortId,
            msg,
			   len,
            INFINITE_TIME_VALUE,
            &RetCode );

    if(  RetCode != NO_ERROR )
    {
      WriteLogErrorFunc( RetCode,
      "WriteResultsFromAuxPartition", "SEND_QUEUING_MESSAGE" );
    }
  }
  else
  {
    WriteLogErrorFunc( RetCode,
           "WriteResultsFromAuxPartition", "GET_QUEUING_PORT_ID" );
  }
  return RetCode;

} /* WriteResultsFromAuxPartition */








/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: EvaluateResult                                      | *
 * |                                                                    | *
 * | DESCRIPTION: Receive a boolean value, and return OK or FAIL        | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | BOOLEAN FlagOK                  X                                  | *
 * |                                                                    | *
 * | RETURN VALUE:  TEST_RESULT_TYPE TestResult                         | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
TEST_RESULT_TYPE EvaluateResult( BOOLEAN FlagOK )
{
  int TestResult;

  if( FlagOK == TRUE )
  {
    TestResult = SUCESS;
    printf("SUCCESS\n");
  }
  else
  {
    TestResult = FAILED;
    printf("FAILED\n");
  }
  
  return TestResult;

} /* EvaluateResult */




/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: EvaluatePriority                                    | *
 * |                                                                    | *
 * | DESCRIPTION: Compare two int   s                                   | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | long Priority1                  X                                  | *
 * | long Priority2                  X                                  | *
 * | BOOLEAN *FlagOK                 X      X                           | +
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
EvaluatePriority( long Priority1, long Priority2, BOOLEAN *FlagOK )
{

  if( Priority1 != Priority2 )
  {
    *FlagOK = FALSE;

    printf(
       "Test(%03d) %s - "
       "Function EvaluatePriority. "
       "Expected PRIORITY equal to '%ld', but obtains '%ld'.\n",
       NrTestId,
       GetTestName(NrTestId),
       Priority1,
       Priority2 );
  }
}  /* EvaluatePriority */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: EvaluateMessage                                     | *
 * |                                                                    | *
 * | DESCRIPTION: Compare two strings                                   | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | char * str1                     X                                  | *
 * | char * str2                     X                                  | *
 * | BOOLEAN *Result                 X      X                           | +
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
EvaluateMessage( char * str1, char * str2, BOOLEAN *Result  )
{
  if( strcmp( str1, str2 ) != 0)
  {
    *Result = FALSE;
    printf(
       "Test(%03d) %s - "
       "Function EvaluateMessage. "
       "Expected Message like '%s', but obtains '%s'.\n",
       NrTestId,
       GetTestName(NrTestId),
       str2,
       str1 );

  }
} /* EvaluateMessage */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: PutOkFailInMessage                                  | *
 * |                                                                    | *
 * | DESCRIPTION: Evaluate boolean value, if TRUE, write in string "OK" | *
 * |              otherwise write "FAIL".                               | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | BOOLEAN FlagOK                  X                                  | *
 * | char * Message                         X                           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
PutOkFailInMessage( BOOLEAN FlagOK, char * Message )
{
  if( FlagOK == TRUE )
  {
   // strcpy( Message, "OK" );
	  memcpy( Message, "OK", 2 );
  }
  else
  {
    //strcpy( Message, "FAIL" );
	  memcpy( Message, "FAIL", 4 );
  }
} /* PutOkFailInMessage */







/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: EvaluateError                                       | *
 * |                                                                    | *
 * | DESCRIPTION: Evaluate RetCode value, if NO_ERROR set FlagOK to TRUE| *
 * |              otherwise set FlagOK to FALSE                         | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | RETURN_CODE_TYPE RetCode        X                                  | *
 * |BOOLEAN *FlagOK                                                     | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
EvaluateError( RETURN_CODE_TYPE RetCode, BOOLEAN *FlagOK  )
{
  if( RetCode != NO_ERROR )
  {

    *FlagOK = FALSE;
  }

} /* EvaluateError */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: EvaluateErrorType                                   | *
 * |                                                                    | *
 * | DESCRIPTION: Compare RetCode and Error values, if they are de same | *
 * |              set FlagOK to TRUE, otherwise set FlagOK to FALSE     | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | RETURN_CODE_TYPE RetCode        X                                  | *
 * | BOOLEAN *FlagOK                        X                           | *
 * | RETURN_CODE_TYPE Error          X                                  | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
EvaluateErrorType( RETURN_CODE_TYPE RetCode,
       BOOLEAN *FlagOK,
       RETURN_CODE_TYPE Error )
{
  if( RetCode != Error )
  {
    *FlagOK = FALSE;
  }

} /* EvaluateErrorType */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: EvaluateVariable                                    | *
 * |                                                                    | *
 * | DESCRIPTION: Compare the value of GV1 and value. if the same, set  | *
 * |              FlagOK to TRUE, otherwise set FlagOK to FALSE.        | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | BOOLEAN GV1val                     X                                  | *
 * | BOOLEAN value                   X                                  | *
 * | BOOLEAN *FlagOK                         X                          | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
EvaluateVariable( BOOLEAN GV1val, BOOLEAN value,  BOOLEAN *FlagOK )
{
  char *TabBoolString[2] = { "FALSE", "TRUE" };
  
  if( GV1val != value )
  {
    *FlagOK = FALSE;
	 
    printf(
             "Test(%03d) %s - "
              "Function EvaluateVariable. "
              "Expected boolean variable like '%s', but obtains '%s'.\n",
              NrTestId,
              GetTestName(NrTestId),
              TabBoolString[value],
              TabBoolString[GV1val] );

  }

} /* EvaluateVariable */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: EvaluateNumValue                                    | *
 * |                                                                    | *
 * | DESCRIPTION: Compare the value of value1 and value2. if the same,  | *
 * |              set FlagOK to TRUE, otherwise set FlagOK to FALSE.    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | int value1                      X                                  | *
 * | int value2                      X                                  | *
 * | BOOLEAN *FlagOK                        X                           | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
EvaluateNumValue( int value1, int value2, BOOLEAN *FlagOK )
{
  if( value1 != value2 )
  {
    *FlagOK = FALSE;

    printf(
       "Test(%03d) %s - "
       "Function EvaluateProcessState. "
       "Expected value like '%d', but obtains '%d'.\n",
       NrTestId,
       GetTestName(NrTestId),
       value2,
       value1 );

  }

} /* EvaluateNumValue */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME:WriteLogReturnCode                                   | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:                                                              | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
WriteLogReturnCode( char             *Function,
                    RETURN_CODE_TYPE  ExpError,
                    RETURN_CODE_TYPE  ObtError )
{
  char wstr[20];
  char wstr1[20];

  if ( TraceActive == AVT_ACTIVE )
  {
     GetErrorName( ExpError, wstr);
     GetErrorName( ObtError, wstr1);

     printf (
               "Test(%03d) %s - "
               "Function %s "
               "expected %s "
               "but obtains %s error. \n",
               NrTestId ,
               GetTestName(NrTestId),
               Function,
               wstr,
               wstr1 );
	}

} /* WriteLogReturnCode */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: WriteLogErrorFunc                                   | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:                                                              | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
WriteLogErrorFunc(  RETURN_CODE_TYPE RetCode,
                    char *FunctionName,
                    char *Action)
{
  char wstr[20];

 // if ( TraceActive == AVT_ACTIVE )
  //{
     GetErrorName( RetCode, wstr);

     printf(
              "Test(%03d) %s - "
              "Function %s. "
              "Error %s while attempting to do %s.\n",
              NrTestId,
              GetTestName(NrTestId),
              FunctionName,
              wstr,
              Action);
   //}
}  /* WriteLogErrorFunc */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: WriteLogErrorNum                                   | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:                                                              | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
WriteLogErrorNum( char *FunctionName,
                  char *Param,
                  long ExpectedValue,
                  long ObtainedValue)
{
  if ( TraceActive == AVT_ACTIVE )
  {
     printf(
              "Test(%03d) %s - "
              "Function %s. "
              "Expected %s equal to %ld, but obtains %ld.\n",
              NrTestId,
              GetTestName(NrTestId),
              FunctionName,
              Param,
              ExpectedValue,
              ObtainedValue );
   }
}  /* WriteLogErrorNum */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: WriteLogErrorStr                                    | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:                                                              | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
7 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
WriteLogErrorStr( char      *FunctionName,
                  char      *Param,
                  MESSAGE_ADDR_TYPE ExpectedMsg,
                  MESSAGE_ADDR_TYPE ObtainedMsg )
{
  if ( TraceActive == AVT_ACTIVE )
  {
     printf(
              "Test(%03d) %s - "
              "Function %s. "
              "Expected %s like '%s', but obtains '%s'.\n",
              NrTestId,
              GetTestName(NrTestId),
              FunctionName,
              Param,
              ExpectedMsg,
              ObtainedMsg );
  }
}  /* WriteLogErrorStr */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: GetErrorName                                        | *
 * |                                                                    | *
 * | DESCRIPTION:                                                       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:                                                              | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:  RETURN_CODE_TYPE RetCode                            | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
GetErrorName( RETURN_CODE_TYPE RetCode, char *ErrorName )
{
  char *TabError[] = {"NO_ERROR","NO_ACTION","NOT_AVAILABLE","INVALID_PARAM",
          "INVALID_CONFIG","INVALID_MODE","TIMED_OUT" };

  strcpy( ErrorName, TabError[RetCode] );

} /*  GetErrorName  */




/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                    END OF FILE: AVT_Funcs.c                        | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */


