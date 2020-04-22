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
 * | FILE: AVT_Wrapper.c                                                | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVTWRAPPER                                              | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S):  w_TIMED_WAIT                               | * 
 * |                         w_PERIODIC_WAIT                            | *
 * |                         w_GET_TIME                                 | *
 * |                         w_REPLENISH                                | *
 * |                         w_CREATE_PROCESS                           | *
 * |                         w_SET_PRIORITY                             | *
 * |                         w_SUSPEND                                  | *
 * |                         w_RESUME                                   | *
 * |                         w_STOP                                     | *
 * |                         w_START                                    | *
 * |                         w_DELAYED_START                            | *
 * |                         w_LOCK_PREEMPTION                          | *
 * |                         w_UNLOCK_PREEMPTION                        | *
 * |                         w_GET_MY_ID                                | *
 * |                         w_GET_PROCESS_ID                           | *
 * |                         w_GET_PROCESS_STATUS                       | *
 * |                         w_GET_PARTITION_STATUS                     | *
 * |                         w_SET_PARTITION_MODE                       | *
 * |                         w_CREATE_LOGBOOK                           | *
 * |                         w_READ_LOGBOOK                             | *
 * |                         w_WRITE_LOGBOOK                            | *
 * |                         w_CLEAR_LOGBOOK                            | *
 * |                         w_GET_LOGBOOK_ID                           | *
 * |                         w_CREATE_SAMPLING_PORT                     | *
 * |                         w_WRITE_SAMPLING_MESSAGE                   | *
 * |                         w_READ_SAMPLING_MESSAGE                    | *
 * |                         w_GET_SAMPLING_PORT_ID                     | *
 * |                         w_GET_SAMPLING_PORT_STATUS                 | *
 * |                         w_CREATE_QUEUING_PORT                      | *
 * |                         w_SEND_QUEUING_MESSAGE                     | *
 * |                         w_RECEIVE_QUEUING_MESSAGE                  | *
 * |                         w_GET_QUEUING_PORT_ID                      | *
 * |                         w_GET_QUEUING_PORT_STATUS                  | *
 * |                         w_CREATE_BUFFER                            | *
 * |                         w_SEND_BUFFER                              | *
 * |                         w_RECEIVE_BUFFER                           | *
 * |                         w_GET_BUFFER_ID                            | *
 * |                         w_GET_BUFFER_STATUS                        | *
 * |                         w_CREATE_BLACKBOARD                        | *
 * |                         w_DISPLAY_BLACKBOARD                       | *
 * |                         w_READ_BLACKBOARD                          | *
 * |                         w_CLEAR_BLACKBOARD                         | *
 * |                         w_GET_BLACKBOARD_ID                        | *
 * |                         w_GET_BLACKBOARD_STATUS                    | *
 * |                         w_CREATE_SEMAPHORE                         | *
 * |                         w_WAIT_SEMAPHORE                           | *
 * |                         w_SIGNAL_SEMAPHORE                         | *
 * |                         w_GET_SEMAPHORE_ID                         | *
 * |                         w_GET_SEMAPHORE_STATUS                     | *
 * |                         w_CREATE_EVENT                             | *
 * |                         w_SET_EVENT                                | *
 * |                         w_RESET_EVENT                              | *
 * |                         w_WAIT_EVENT                               | *
 * |                         w_GET_EVENT_ID                             | *
 * |                         w_GET_EVENT_STATUS                         | *
 * |                         w_CREATE_ERROR_HANDLER                     | *
 * |                         w_GET_ERROR_STATUS                         | *
 * |                         w_RAISE_APPLICATION_ERROR                  | *
 * |                         w_REPORT_APPLICATION_MESSAGE               | *
 * /                                                                    / *
 * |                         DisplayHeader                              | * 
 * |                         ValidateValue                              | *
 * |                         ValidateString                             | *
 * |                                                                    | *
 * | DESCRIPTION: wrapper functions with trace                          | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    25/09/2002...                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "AVT_Wrapper.h"
#include <string.h>


static char *TabError[] = { "NO_ERROR", "NO_ACTION", "NOT_AVAILABLE",
			    "INVALID_PARAM", "INVALID_CONFIG",
			    "INVALID_MODE", "TIMED_OUT" };

static char *TabErrorNames[] = { "DEADLINE_MISSED", "APPLICATION_ERROR", "NUMERIC_ERROR",
		       "ILLEGAL_REQUEST", "STACK_OVERFLOW", "MEMORY_VIOLATION",
		       "HARDWARE_FAULT", "POWER_FAIL", "INVALID_ERROR" };
int TraceActive;
int NrTestId;




/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_GET_PARTITION_STATUS                              | *
 * |                                                                    | *
 * | DESCRIPTION: Execute  GET_PARTITION_STATUS with trace              | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like  GET_PARTITION_STATUS                                         | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
w_GET_PARTITION_STATUS( PARTITION_STATUS_TYPE *PrtStatus,
			RETURN_CODE_TYPE      *RetCode )
{
  char *TabOper[] = { "IDLE", "COLD_STAR", "WARM_START", "NORMAL" };

  printf("w_GET_PARTITION_STATUS\n");

  /* execute service */
  GET_PARTITION_STATUS( PrtStatus, RetCode );

  /* Display trace */
  if( TraceActive == AVT_ACTIVE )
  {
   /* printf(
		   "Test(%03d) %s - "
		   "Service GET_PARTITION_STATUS( "
		   "IDENTIFIER     : %ld \n"
		   "%63s PERIOD         : %lld \n"
		   "%63s DURATION       : %lld \n",
		   NrTestId,
		   GetTestName(NrTestId),
		   PrtStatus->IDENTIFIER,
		   "", PrtStatus->PERIOD,
		   "", PrtStatus->DURATION );*/
    
    printf(
		   "_%63s LOCK_LEVEL     : %ld \n"
		   "%63s OPERATING_MODE : %s \n"
		   "%63s RETURN_CODE    : %s ) \n",
		   "", PrtStatus->LOCK_LEVEL,
		   "", TabOper[PrtStatus->OPERATING_MODE],
		   "", TabError[*RetCode] );
    
  }
 
}  /* w_GET_PARTITION_STATUS */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_SET_PARTITION_MODE                                | *
 * |                                                                    | *
 * | DESCRIPTION: Execute  GET_PARTITION_MODE with trace                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like  GET_PARTITION_STATUS                                         | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
w_SET_PARTITION_MODE( OPERATING_MODE_TYPE OperMode,
		      RETURN_CODE_TYPE    *RetCode )
{
  char *TabOper[] = { "IDLE", "COLD_STAR", "WARM_START", "NORMAL" };

  
  printf("w_SET_PARTITION_MODE (%d)\n", OperMode);
  /* execute service */
  SET_PARTITION_MODE( OperMode, RetCode );

  /* Display trace */
  if( TraceActive == AVT_ACTIVE )
  {
    /* Trace init */
    DisplayHeader( "INIT", "SET_PARTITION_MODE" );
    printf(
		   "OPERATING_MODE : %s )\n",
		   TabOper[OperMode] );
    
    /* Trace end */
    DisplayHeader( "END ", "SET_PARTITION_MODE" );
    printf(
		   "RETURN_CODE    : %s )\n",
		   TabError[*RetCode] );
  }
}  /* w_SET_PARTITION_MODE */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_CREATE_SAMPLING_PORT                              | *
 * |                                                                    | *
 * | DESCRIPTION: Execute CREATE_SAMPLING_PORT with trace               | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like CREATE_SAMPLING_PORT                                          | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
w_CREATE_SAMPLING_PORT( SAMPLING_PORT_NAME_TYPE Name,
			MESSAGE_SIZE_TYPE       Length,
			PORT_DIRECTION_TYPE     Direction,
			SYSTEM_TIME_TYPE        Refresh,
			SAMPLING_PORT_ID_TYPE  *PortId,
			RETURN_CODE_TYPE       *RetCode )
{
  char *TabDir[] = { "SOURCE", "DESTINATION", " " };
  long wDirection;
  
  printf("w_CREATE_SAMPLING_PORT %s\n", Name);

  /* execute service */
  CREATE_SAMPLING_PORT( Name, Length, Direction, Refresh, PortId, RetCode );
  
  /* Display trace */
  if( TraceActive == AVT_ACTIVE )
  {
    /* validate direction */
    wDirection = Direction;
    ValidateValue( &wDirection );
    
    /* Trace init */
    DisplayHeader( "INIT", "CREATE_SAMPLING_PORT" );
    printf(
		   "SAMPLING_PORT_NAME : %s \n"
		   "%68s MAX_MESSAGE_SIZE   : %ld \n"
		   "%68s PORT_DIRECTION     : %s \n"
		   "%68s REFRESH_PERIOD     : %lld )\n",
		   Name,
		   "", Length,
		   "", TabDir[wDirection],
		   "", Refresh );

	 if ( *RetCode == NO_ERROR )
	 {
       /* Trace end */
       DisplayHeader( "END ", "CREATE_SAMPLING_PORT" );
       printf(
		          "SAMPLING_PORT_ID   : %ld \n"
		          "%68s RETURN_CODE        : %s )\n",
		          *PortId,
		          "", TabError[*RetCode] );
	 }
	 else
	 {
       /* Trace end */
       DisplayHeader( "END ", "CREATE_SAMPLING_PORT" );
       printf(
		          "%68s RETURN_CODE        : %s )\n",
		          "", TabError[*RetCode] );		 
	 }
  }
}  /* w_CREATE_SAMPLING_PORT */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_WRITE_SAMPLING_MESSAGE                            | *
 * |                                                                    | *
 * | DESCRIPTION: Execute WRITE_SAMPLING_MESSAGE  with trace            | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like WRITE_SAMPLING_MESSAGE                                        | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
w_WRITE_SAMPLING_MESSAGE( SAMPLING_PORT_ID_TYPE PortId,
			  MESSAGE_ADDR_TYPE     Message,
			  MESSAGE_SIZE_TYPE     Length,
			  RETURN_CODE_TYPE      *RetCode )
{
	printf("w_WRITE_SAMPLING_MESSAGE %s \n", Message);

  WRITE_SAMPLING_MESSAGE( PortId, Message, Length, RetCode );

  /* Display trace */
  if( TraceActive == AVT_ACTIVE )
  {
    /* Trace init */
    DisplayHeader( "INIT", "WRITE_SAMPLING_MESSAGE" );
    printf(
		   "SAMPLING_PORT_ID : %ld \n"
		   "%70s MESSAGE          : %s \n"
		   "%70s MESSAGE_SIZE     : %ld )\n",
		   PortId,
		   "", Message,
		   "", Length );

    /* Trace end */
    DisplayHeader( "END ", "WRITE_SAMPLING_MESSAGE" );
    printf(
		   "RETURN_CODE      : %s )\n",
		   TabError[*RetCode] );
  }
} /* w_WRITE_SAMPLING_MESSAGE */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_READ_SAMPLING_MESSAGE                             | *
 * |                                                                    | *
 * | DESCRIPTION: Execute READ_SAMPLING_MESSAGE with trace              | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like READ_SAMPLING_MESSAGE                                         | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
w_READ_SAMPLING_MESSAGE( SAMPLING_PORT_ID_TYPE PortId,
			 MESSAGE_ADDR_TYPE     Message,
			 MESSAGE_SIZE_TYPE    *Length,
			 VALIDITY_TYPE        *Validity,
			 RETURN_CODE_TYPE     *RetCode )
{
  char *TabVal[] = { "INVALID", "VALID", " " };
  char wMessage[256];
  long wValidity;

  printf("w_READ_SAMPLING_MESSAGE\n");

  /* Execute service */
  READ_SAMPLING_MESSAGE( PortId, Message, Length, Validity, RetCode );

   /* Display trace */
  if( TraceActive == AVT_ACTIVE )
  {
    /* Validate validity value */
    wValidity = *Validity;
    ValidateValue( &wValidity );

    /* validity message */
    memset( wMessage, '\0', sizeof( wMessage ));
    ValidateString( Message, wMessage, *Length );

    /* Trace init */
    DisplayHeader( "INIT", "READ_SAMPLING_MESSAGE" );
    printf(
		   "SAMPLING_PORT_ID : %ld )\n",
		   PortId );

    /* Trace end */
    DisplayHeader( "END ", "READ_SAMPLING_MESSAGE" );
	 if ( *RetCode == NO_ERROR )
	 {
       printf(
	  	          "MESSAGE          : %s \n"
		          "%69s MESSAGE_SIZE     : %ld \n"
		          "%69s VALIDITY         : %s \n"
		          "%69s RETURN_CODE      : %s )\n",
		          wMessage,
		          "", *Length,
		          "", TabVal[wValidity],
		          "", TabError[*RetCode] );
	 }
	 else
	 {
       printf(
		          "%69s RETURN_CODE      : %s )\n",
		          "", TabError[*RetCode] );
	 }

  }
}  /* w_READ_SAMPLING_MESSAGE */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_GET_SAMPLING_PORT_ID                              | *
 * |                                                                    | *
 * | DESCRIPTION: Execute GET_SAMPLING_PORT_ID with trace               | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like GET_SAMPLING_PORT_ID                                          | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
w_GET_SAMPLING_PORT_ID( SAMPLING_PORT_NAME_TYPE PortName,
			SAMPLING_PORT_ID_TYPE   *PortId,
			RETURN_CODE_TYPE        *RetCode )
{
  char Name[30];

  printf("w_GET_SAMPLING_PORT_ID %s\n", PortName);

  /* Execute service */
  GET_SAMPLING_PORT_ID( PortName, PortId, RetCode );
  /* Display trace */
  if( TraceActive == AVT_ACTIVE )
  {
    /* Validate name */
    memset( Name, '\0', sizeof( Name ));
    ValidateString((unsigned char *) PortName, Name, strlen( PortName ));

    /* Trace init */
    DisplayHeader( "INIT", "GET_SAMPLING_PORT_ID" );
    printf( "PORT_NAME   : %s )\n", Name );
    
    /* Trace end */
    DisplayHeader( "END ", "GET_SAMPLING_PORT_ID" );
	 if ( *RetCode == NO_ERROR )
	 {
       printf(
		          "PORT_ID     : %ld \n"
		          "%68s RETURN_CODE : %s )\n",
		          *PortId,
		          "", TabError[*RetCode] );    
	 }
	 else
	 {
       printf(
		          "%68s RETURN_CODE : %s )\n",
		          "", TabError[*RetCode] );    		 
	 }
  }
} /* w_GET_SAMPLING_PORT_ID */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_GET_SAMPLING_PORT_STATUS                          | *
 * |                                                                    | *
 * | DESCRIPTION: Execute READ_SAMPLING_MESSAGE with trace              | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like GET_SAMPLING_PORT_STATUS                                      | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
w_GET_SAMPLING_PORT_STATUS( SAMPLING_PORT_ID_TYPE      PortId,
			    SAMPLING_PORT_STATUS_TYPE  *PortStatus,
			    RETURN_CODE_TYPE           *RetCode )
{
  char *TabDir[] = { "SOURCE", "DESTINATION", " " };
  char *TabVal[] = { "INVALID", "VALID", " " };
  long wDirection;
  long wValidity;

  printf("w_GET_SAMPLING_PORT_STATUS %d\n", PortId);

  /* execute service */
  GET_SAMPLING_PORT_STATUS( PortId, PortStatus, RetCode );



  /* Display trace */
  if( TraceActive == AVT_ACTIVE )
  {
    /* validate port direction  and  validity value */
    wDirection = PortStatus->PORT_DIRECTION;
    ValidateValue( &wDirection);
    wValidity = PortStatus->LAST_MSG_VALIDITY;
    ValidateValue( &wValidity );

    /* Trace init */
    DisplayHeader( "INIT", "GET_SAMPLING_PORT_STATUS" );
    printf(
       "PORT_Id           : %ld )\n",
       PortId );

    /* Trace end */
    DisplayHeader( "END ", "GET_SAMPLING_PORT_STATUS" );
	 if ( *RetCode == NO_ERROR )
	 {
       printf(
                "MAX_MESSAGE_SIZE  : %ld \n"
                "%72s PORT_DIRECTION    : %s  \n",
                PortStatus->MAX_MESSAGE_SIZE,
                "", TabDir[wDirection] );
	 
       /*printf(
                "%72s REFRESH_PERIOD    : %lld \n"
                "%72s LAST_MSG_VALIDITY : %s \n"
                "%72s RETURN_CODE       : %s )\n",
                "", PortStatus->REFRESH_PERIOD,
                "", TabVal[wValidity],
                "", TabError[*RetCode] );*/
	 }
	 else
	 {
       printf(
                "%72s RETURN_CODE       : %s )\n",
                "", TabError[*RetCode] );
		 
	 }
  }
}  /* w_GET_SAMPLING_PORT_STATUS  */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_CREATE_QUEUING_PORT                               | *
 * |                                                                    | *
 * | DESCRIPTION: Execute CREATE_QUEUING_PORT with trace                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like CREATE_QUEUING_PORT                                           | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
w_CREATE_QUEUING_PORT( QUEUING_PORT_NAME_TYPE  PortName,
		       MESSAGE_SIZE_TYPE       Length,
		       MESSAGE_RANGE_TYPE      Range,
		       PORT_DIRECTION_TYPE     Direction,
		       QUEUING_DISCIPLINE_TYPE Discipline,
		       QUEUING_PORT_ID_TYPE    *PortId,
		       RETURN_CODE_TYPE        *RetCode )
{
  char *TabDir[] = { "SOURCE", "DESTINATION", "INVALID_DIRECTION" };
  char *TabDsp[] = { "FIFO", "PRIORITY", "INVALID_DISCIPLINE" };
  char wName[30];
  long wDirection = 0;
  long wDiscipline = 0;

  printf("w_CREATE_QUEUING_PORT %s\n", PortName);

  /* execute service */
  CREATE_QUEUING_PORT( PortName,
		       Length,
		       Range,
		       Direction,
		       Discipline,
		       PortId,
		       RetCode);

  if(*RetCode == NO_ERROR) printf("NO_ERROR\n");
  if(*RetCode == NO_ACTION) printf("NO_ACTION\n");
  if(*RetCode == NOT_AVAILABLE) printf("NOT_AVAILABLE\n");
  if(*RetCode == INVALID_PARAM) printf("INVALID_PARAM\n");
  if(*RetCode == INVALID_CONFIG) printf("INVALID_CONFIG\n");
  if(*RetCode == INVALID_MODE) printf("INVALID_MODE\n");
  if(*RetCode == TIMED_OUT) printf("TIMED_OUT\n");


  /* Display trace */
  if( TraceActive == AVT_ACTIVE )
  {
    /* validate direction and discipline*/
    wDirection = Direction;
    ValidateValue( &wDirection );
    wDiscipline = Discipline;
    ValidateValue( &wDiscipline );

    /* validate name */
    memset( wName, '\0', sizeof( wName ));
    ValidateString((unsigned char *) PortName, wName, strlen( PortName ));

    /* Trace init */
    DisplayHeader( "INIT", "CREATE_QUEUING_PORT" );
    printf(
		   "QUEUING_PORT_NAME : %s \n"
		   "%67s MAX_MESSAGE_SIZE   : %ld \n"
		   "%67s MAX_NB_MESSAGE     : %ld \n",
		   wName,
		   "", Length,
		   "", Range );
    
    printf(
		   "%67s PORT_DIRECTION     : %s  \n"
		   "%67s QUEUING_DISCIPLINE : %s )\n",
		   "",TabDir[wDirection],
		   "",TabDsp[wDiscipline] );

    /* Trace end */
    DisplayHeader( "END ", "CREATE_QUEUING_PORT" );
	 if ( *RetCode == NO_ERROR )
	 {
       printf(
	    	       "QUEUING_PORT_ID    : %ld \n"
		          "%67s RETURN_CODE        : %s )\n",
		          *PortId,
		          "", TabError[*RetCode] );
	 }
	 else
	 {
       printf(
		          "%67s RETURN_CODE        : %s )\n",
		          "", TabError[*RetCode] );
	 }	 
  }
}  /* w_CREATE_QUEUING_PORT */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_SEND_QUEUING_MESSAGE                              | *
 * |                                                                    | *
 * | DESCRIPTION: Execute SEND_QUEUING_MESSAGE with trace               | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like SEND_QUEUING_MESSAGE                                          | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
w_SEND_QUEUING_MESSAGE( QUEUING_PORT_ID_TYPE PortId,
			MESSAGE_ADDR_TYPE    Message,
			MESSAGE_SIZE_TYPE    Length,
			SYSTEM_TIME_TYPE     TimeOut,
			RETURN_CODE_TYPE    *RetCode )
{
  char wMessage[256];
  printf("w_SEND_QUEUING_MESSAGE %s (%d)\n", Message, Length);

  /* execute service */
  SEND_QUEUING_MESSAGE( PortId, Message, Length, TimeOut, RetCode );


  /* Display trace */
  if( TraceActive == AVT_ACTIVE )
  {
    /* validate Message */
    memset( wMessage, '\0', sizeof( wMessage ));
    ValidateString( Message, wMessage, Length);

    /* Trace init */
    DisplayHeader( "INIT", "SEND_QUEUING_MESSAGE" );
    printf(
		   "QUEUING_PORT_ID : %ld \n"
		   "%68s MESSAGE         : %s  \n"
		   "%68s MESSAGE_SIZE    : %ld \n"
		   "%68s TIME_OUT        : %lld )\n",
		   PortId,
		   "", wMessage,
		   "", Length,
		   "", TimeOut );
    
    /* Trace end */
    DisplayHeader( "END ", "SEND_QUEUING_MESSAGE" );
    printf(
		   "RETURN_CODE     : %s )\n",
		   TabError[*RetCode] );
  }
}  /* w_SEND_QUEUING_MESSAGE */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_RECEIVE_QUEUING_MESSAGE                           | *
 * |                                                                    | *
 * | DESCRIPTION: Execute SEND_QUEUING_MESSAGE with trace               | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like RECEIVE_QUEUING_MESSAGE                                       | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
w_RECEIVE_QUEUING_MESSAGE( QUEUING_PORT_ID_TYPE PortId,
			   SYSTEM_TIME_TYPE     TimeOut,
			   MESSAGE_ADDR_TYPE    Message,
			   MESSAGE_SIZE_TYPE   *Length,
			   RETURN_CODE_TYPE    *RetCode )
{
  char wMessage[256];

  printf("w_RECEIVE_QUEUING_MESSAGE %ld\n", TimeOut);

  /* execute service */
  RECEIVE_QUEUING_MESSAGE( PortId, TimeOut, Message, Length, RetCode );
		
  /* Display trace */
  if( TraceActive == AVT_ACTIVE )
  {
    /* validate Message */
    memset( wMessage, '\0', sizeof( wMessage ));
    ValidateString( Message, wMessage, *Length);

    /* Trace init */
    DisplayHeader( "INIT", "RECEIVE_QUEUING_MESSAGE" );
    printf(
		   "QUEUING_PORT_ID : %ld \n"
		   "%71s TIME_OUT        : %lld )\n",
		   PortId,
		   "", TimeOut );

    /* Trace end */
       DisplayHeader( "END ", "RECEIVE_QUEUING_MESSAGE" );
	 if ( *RetCode == NO_ERROR )
	 {
       printf(
		          "MESSAGE         : %s  \n"
		          "%71s MESSAGE_SIZE    : %ld \n"
		          "%71s RETURN_CODE     : %s )\n",
		          wMessage,
		          "", *Length,
		          "", TabError[*RetCode] );
	 }
	 else
	 {
       printf(
		          "%71s RETURN_CODE     : %s )\n",
		          "", TabError[*RetCode] );		 
	 }
  }

}  /* w_RECEIVE_QUEUING_MESSAGE */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_GET_QUEUING_PORT_ID                               | *
 * |                                                                    | *
 * | DESCRIPTION: Execute GET_QUEUING_PORT_ID with trace                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like GET_QUEUING_PORT_ID                                           | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
w_GET_QUEUING_PORT_ID( QUEUING_PORT_NAME_TYPE PortName,
		       QUEUING_PORT_ID_TYPE   *PortId,
		       RETURN_CODE_TYPE        *RetCode )
{
  char Name[30];

  printf("w_GET_QUEUING_PORT_ID %s\n", PortName);

  /* Execute service */
  GET_QUEUING_PORT_ID( PortName, PortId, RetCode );

  /* Display trace */
  if( TraceActive == AVT_ACTIVE )
  {


    /* Validate name */
    memset( Name, '\0', sizeof( Name ));
    ValidateString( (unsigned char *)PortName, Name, strlen( PortName ));

    /* Trace init */
    DisplayHeader( "INIT", "GET_QUEUING_PORT_ID" );
    printf(
		   "PORT_NAME   : %s )\n",
		   Name );

    /* Trace end */
    DisplayHeader( "END ", "GET_QUEUING_PORT_ID" );
	 if ( *RetCode == NO_ERROR )
	 {
       printf(
	       	    "PORT_ID     : %ld \n"
		          "%67s RETURN_CODE : %s )\n",
		          *PortId,
		          "", TabError[*RetCode] );
	 }
	 else
	 {
       printf(
		          "%67s RETURN_CODE : %s )\n",
		          "", TabError[*RetCode] );
	 }
		 
  }
} /* w_GET_QUEUING_PORT_ID */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_GET_QUEUING_PORT_STATUS                           | *
 * |                                                                    | *
 * | DESCRIPTION: Execute READ_QUEUING_MESSAGE with trace               | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like GET_QUEUING_PORT_STATUS                                       | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
w_GET_QUEUING_PORT_STATUS( QUEUING_PORT_ID_TYPE      PortId,
			   QUEUING_PORT_STATUS_TYPE  *PortStatus,
			   RETURN_CODE_TYPE           *RetCode )
{
  char *TabDir[] = { "SOURCE", "DESTINATION", " " };
  long wDirection;

  /* execute service */
  GET_QUEUING_PORT_STATUS( PortId, PortStatus, RetCode );

  /* Display trace */
  if( TraceActive == AVT_ACTIVE )
  {
    /* validate port direction  and  validity value */
    wDirection = PortStatus->PORT_DIRECTION;
    ValidateValue( &wDirection);
    
    /* Trace init */
    DisplayHeader( "INIT", "GET_QUEUING_PORT_STATUS" );
    printf(
		   "PORT_Id           : %ld )\n",
		   PortId );
    
    /* Trace end */
    DisplayHeader( "END ", "GET_QUEUING_PORT_STATUS" );
	 if ( *RetCode == NO_ERROR )
	 {
       printf(
	        	    "NB_MESSAGE        : %ld \n"
		     	    "%71s MAX_NB_MESSAGE    : %ld \n"
		     	    "%71s MAX_MESSAGE_SIZE  : %ld \n",
		     	    PortStatus->NB_MESSAGE,
		     	    "", PortStatus->MAX_NB_MESSAGE,
		     	    "", PortStatus->MAX_MESSAGE_SIZE );
		   
       printf(
		   	    "%71s PORT_DIRECTION    : %s  \n"
		     	    "%71s RETURN_CODE       : %s )\n",
		     	    "", TabDir[wDirection],
		     	    "", TabError[*RetCode] );
	 }
	 else
	 {
       printf(
		     	    "%71s RETURN_CODE       : %s )\n",
		     	    "", TabError[*RetCode] );		 
	 }
  }
}  /* w_GET_QUEUING_PORT_STATUS  */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: DisplayHeader                                       | *
 * |                                                                    | *
 * | DESCRIPTION: Display the header of the message                     | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
DisplayHeader( char *Type, char *Function )
{
  
  printf(
		 "Test(%03d) %s - "
		 "%s Service %s( ",
		 NrTestId,
		 GetTestName(NrTestId),
		 Type, 
		 Function );

} /* DisplayHeader */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: ValidateValue                                       | *
 * |                                                                    | *
 * | DESCRIPTION: Force values to be 0, 1 or 2                          | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
ValidateValue( long *Value  )
{
  if( ( *Value < 0 ) || ( *Value > 1 ) )
  {
    *Value = 2;
  }
} /* ValidateValue */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: ValidateString                                      | *
 * |                                                                    | *
 * | DESCRIPTION: Validate a string in order to allow null value        | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
ValidateString(  unsigned char *OldStr, char *NewStr, long Length )
{
  if( Length <= 0 )
  {
    strcpy( NewStr, "NULL" );
  }
  else
  {
    strncpy( NewStr, OldStr, Length );
    NewStr[Length] = '\0';
  }
} /* ValidateString  */





/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                       END OF FILE: AVT_Wrapper.c                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
