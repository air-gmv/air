/** 
 * @file AVT_independent.c
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  AVT functions original implementations
 * @note The functions contained in this file should be compatible
 *    with any target or implementation supporting the ARINC-653 
 *    Specification.
 * 
 */

#include "AVT_independent.h"

 /* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: print_testdef_result                                     | *
 * |                                                                    | *
 * | DESCRIPTION: Print Test results based on TEST_RESULT_TYPE input    | *
 * |              using GMV TESTDEF format                              | *
 * | AUTHOR: Bruno Oliveira                                             | *
 * | DATE:   30/10/2013                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * | INPUT:  TEST_RESULT_TYPE TestResult                                | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

void print_testdef_result(TEST_RESULT_TYPE TestResult) {
  extern char TestDefName[];
  printf("####%s####\n", TestDefName);
  switch (TestResult) {
    case FAILED:
    case NOT_PERFORMED:
      printf("**0**\n");
    break;
    case SUCCESS:
      printf("**1**\n");
    break;
    default:
      printf("Error in print_testdef_result\n");
    break;
  }
}

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
    TestResult = SUCCESS;
    #ifdef DEBUG
    printf("SUCCESS\n");
    #endif
  }
  else
  {
    TestResult = FAILED;
    #ifdef DEBUG
    printf("FAILED\n");
    #endif
  }
  
  return TestResult;

} /* EvaluateResult */




/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_SET_PARTITION_MODE                                | *
 * |                                                                    | *
 * | DESCRIPTION: Execute  SET_PARTITION_MODE                           | *
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

  #ifdef DEBUG
  printf("w_SET_PARTITION_MODE (%d)\n", OperMode);
  #endif
  /* execute service */
  SET_PARTITION_MODE( OperMode, RetCode );

}  /* w_SET_PARTITION_MODE */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: print_test_results                                  | *
 * |                                                                    | *
 * | DESCRIPTION: Prints test results based on a TEST_RESULT_TYPE       | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

void print_test_results(TEST_RESULT_TYPE TestResult){

  extern char TestName[];

  printf("%s : ", TestName);
  if(TestResult == FAILED){
    printf("FAILED\n");
  } else if(TestResult == NOT_PERFORMED){
    printf("NOT_PERFORMED\n");
  } else printf("SUCCESS\n");

}






/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_SET_MODULE_SCHEDULE                               | *
 * |                                                                    | *
 * | DESCRIPTION: Execute SET_MODULE_SCHEDULE                           | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   11/09/2013                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like SET_MODULE_SCHEDULE                                           | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */


void w_SET_MODULE_SCHEDULE(
  SCHEDULE_ID_TYPE SchedId,
  RETURN_CODE_TYPE *ReturnCode
  ) 
{
  #ifdef DEBUG
  printf("w_SET_MODULE_SCHEDULE (%u)", (unsigned int) SchedId);
  #endif
  /*Execute ARINC 653 Service*/
  SET_MODULE_SCHEDULE(SchedId, ReturnCode);

  //  /* Display trace */
  // if( TraceActive == AVT_ACTIVE )
  // {
  //   /* Trace init */
  //   DisplayHeader( "INIT", "SET_MODULE_SCHEDULE" );
  //   printf(
  //      "SCHEDULE_ID : %u )\n",
  //      (unsigned int) SchedId );
    
  //   /* Trace end */
  //   DisplayHeader( "END ", "SET_MODULE_SCHEDULE" );
  //   printf(
  //      "RETURN_CODE    : %s )\n",
  //      TabError[*ReturnCode] );
  // }

} /*w_SET_MODULE_SCHEDULE*/




/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_GET_MODULE_SCHEDULE_STATUS                        | *
 * |                                                                    | *
 * | DESCRIPTION: Execute GET_MODULE_SCHEDULE_STATUS                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   11/09/2013                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | Like SET_MODULE_SCHEDULE                                           | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void w_GET_MODULE_SCHEDULE_STATUS(
  SCHEDULE_STATUS_TYPE *ScheduleStatus,
  RETURN_CODE_TYPE *RetCode) 
{

  #ifdef DEBUG
  printf("w_GET_MODULE_SCHEDULE_STATUS\n");
  #endif
  /*Execute Service*/
  GET_MODULE_SCHEDULE_STATUS( ScheduleStatus, RetCode );

  // /* Display trace */
  // if( TraceActive == AVT_ACTIVE ) {
  //   DisplayHeader( "INIT", "GET_MODULE_SCHEDULE_STATUS" );



  //   DisplayHeader( "END", "GET_MODULE_SCHEDULE_STATUS" );

  //   if ( *RetCode == NO_ERROR ) {
  //     printf(
  //       "TIME_OF_LAST_SCHEDULE_SWITCH : %ld,"
  //       "CURRENT_SCHEDULE : %u,"
  //       "NEXT_SCHEDULE : %u\n",
  //       ScheduleStatus->TIME_OF_LAST_SCHEDULE_SWITCH,
  //       (unsigned int)ScheduleStatus->CURRENT_SCHEDULE,
  //       (unsigned int)ScheduleStatus->NEXT_SCHEDULE
  //     );
  //   }
  //   else {
  //     printf(
  //       "%67s RETURN_CODE : %s )\n",
  //       "", TabError[*RetCode] );
  //   }


  // }
}




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
  //char Name[30];

  #ifdef DEBUG
  printf("w_GET_QUEUING_PORT_ID %s\n", PortName);
  #endif
  /* Execute service */
  GET_QUEUING_PORT_ID( PortName, PortId, RetCode );

  // /* Display trace */
  // if( TraceActive == AVT_ACTIVE )
  // {


  //   /* Validate name */
  //   memset( Name, '\0', sizeof( Name ));
  //   ValidateString( (unsigned char *)PortName, Name, strlen( PortName ));

  //   /* Trace init */
  //   DisplayHeader( "INIT", "GET_QUEUING_PORT_ID" );
  //   printf(
  //      "PORT_NAME   : %s )\n",
  //      Name );

  //   /* Trace end */
  //   DisplayHeader( "END ", "GET_QUEUING_PORT_ID" );
  //  if ( *RetCode == NO_ERROR )
  //  {
  //      printf(
  //             "PORT_ID     : %ld \n"
  //             "%67s RETURN_CODE : %s )\n",
  //             *PortId,
  //             "", TabError[*RetCode] );
  //  }
  //  else
  //  {
  //      printf(
  //             "%67s RETURN_CODE : %s )\n",
  //             "", TabError[*RetCode] );
  //  }
     
  // }
} /* w_GET_QUEUING_PORT_ID */






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
w_RECEIVE_QUEUING_MESSAGE( 
         QUEUING_PORT_ID_TYPE PortId,
         SYSTEM_TIME_TYPE     TimeOut,
         MESSAGE_ADDR_TYPE    Message,
         MESSAGE_SIZE_TYPE   *Length,
         RETURN_CODE_TYPE    *RetCode )
{
  //char wMessage[256];
  #ifdef DEBUG
  printf("w_RECEIVE_QUEUING_MESSAGE %ld\n", (long) TimeOut);
  #endif
  /* execute service */
  RECEIVE_QUEUING_MESSAGE( PortId, TimeOut, Message, Length, RetCode );

    
  // /* Display trace */
  // if( TraceActive == AVT_ACTIVE )
  // {
  //   /* validate Message */
  //   memset( wMessage, '\0', sizeof( wMessage ));
  //   ValidateString( Message, wMessage, *Length);

  //   /* Trace init */
  //   DisplayHeader( "INIT", "RECEIVE_QUEUING_MESSAGE" );
  //   printf(
  //      "QUEUING_PORT_ID : %ld \n"
  //      "%71s TIME_OUT        : %lld )\n",
  //      PortId,
  //      "", TimeOut );

  //   /* Trace end */
  //      DisplayHeader( "END ", "RECEIVE_QUEUING_MESSAGE" );
  //  if ( *RetCode == NO_ERROR )
  //  {
  //      printf(
  //             "MESSAGE         : %s  \n"
  //             "%71s MESSAGE_SIZE    : %ld \n"
  //             "%71s RETURN_CODE     : %s )\n",
  //             wMessage,
  //             "", *Length,
  //             "", TabError[*RetCode] );
  //  }
  //  else
  //  {
  //      printf(
  //             "%71s RETURN_CODE     : %s )\n",
  //             "", TabError[*RetCode] );    
  //  }
  // }

}  /* w_RECEIVE_QUEUING_MESSAGE */






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
  char wName[30] = "";
  long wDirection = 0;
  long wDiscipline = 0;

  #ifdef DEBUG
  printf("w_CREATE_QUEUING_PORT %s\n", PortName);
  #endif
  /* execute service */
  CREATE_QUEUING_PORT( PortName,
           Length,
           Range,
           Direction,
           Discipline,
           PortId,
           RetCode);

  #ifdef DEBUG
  if(*RetCode == NO_ERROR) printf("NO_ERROR\n");
  if(*RetCode == NO_ACTION) printf("NO_ACTION\n");
  if(*RetCode == NOT_AVAILABLE) printf("NOT_AVAILABLE\n");
  if(*RetCode == INVALID_PARAM) printf("INVALID_PARAM\n");
  if(*RetCode == INVALID_CONFIG) printf("INVALID_CONFIG\n");
  if(*RetCode == INVALID_MODE) printf("INVALID_MODE\n");
  if(*RetCode == TIMED_OUT) printf("TIMED_OUT\n");
  #endif

  // /* Display trace */
  // if( TraceActive == AVT_ACTIVE )
  // {
  //   /* validate direction and discipline*/
  //   wDirection = Direction;
  //   ValidateValue( &wDirection );
  //   wDiscipline = Discipline;
  //   ValidateValue( &wDiscipline );

  //   /* validate name */
  //   memset( wName, '\0', sizeof( wName ));
  //   ValidateString((unsigned char *) PortName, wName, strlen( PortName ));

  //   /* Trace init */
  //   DisplayHeader( "INIT", "CREATE_QUEUING_PORT" );
  //   printf(
  //      "QUEUING_PORT_NAME : %s \n"
  //      "%67s MAX_MESSAGE_SIZE   : %ld \n"
  //      "%67s MAX_NB_MESSAGE     : %ld \n",
  //      wName,
  //      "", Length,
  //      "", Range );
    
  //   printf(
  //      "%67s PORT_DIRECTION     : %s  \n"
  //      "%67s QUEUING_DISCIPLINE : %s )\n",
  //      "",TabDir[wDirection],
  //      "",TabDsp[wDiscipline] );

  //   /* Trace end */
  //   DisplayHeader( "END ", "CREATE_QUEUING_PORT" );
  //  if ( *RetCode == NO_ERROR )
  //  {
  //      printf(
  //              "QUEUING_PORT_ID    : %ld \n"
  //             "%67s RETURN_CODE        : %s )\n",
  //             *PortId,
  //             "", TabError[*RetCode] );
  //  }
  //  else
  //  {
  //      printf(
  //             "%67s RETURN_CODE        : %s )\n",
  //             "", TabError[*RetCode] );
  //  }   
  // }
}  /* w_CREATE_QUEUING_PORT */



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

  #ifdef DEBUG
  printf("w_GET_PARTITION_STATUS\n");
  #endif

  /* execute service */
  GET_PARTITION_STATUS( PrtStatus, RetCode );

  // /* Display trace */
  // if( TraceActive == AVT_ACTIVE )
  // {
  // /* printf(
  //     "Test(%03d) %s - "
  //     "Service GET_PARTITION_STATUS( "
  //     "IDENTIFIER     : %ld \n"
  //     "%63s PERIOD         : %lld \n"
  //     "%63s DURATION       : %lld \n",
  //     NrTestId,
  //     GetTestName(NrTestId),
  //     PrtStatus->IDENTIFIER,
  //     "", PrtStatus->PERIOD,
  //     "", PrtStatus->DURATION );*/
    
  //   printf(
  //      "_%63s LOCK_LEVEL     : %ld \n"
  //      "%63s OPERATING_MODE : %s \n"
  //      "%63s RETURN_CODE    : %s ) \n",
  //      "", PrtStatus->LOCK_LEVEL,
  //      "", TabOper[PrtStatus->OPERATING_MODE],
  //      "", TabError[*RetCode] );
    
  // }
 
}  /* w_GET_PARTITION_STATUS */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_GET_TIME                                          | *
 * |                                                                    | *
 * | DESCRIPTION: Execute GET_TIME with trace                           | *
 * |                                                                    | *
 * | AUTHOR:   Bruno Oliveira                                           | *
 * | DATE:   20/09/2013                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                                                        | *
 * |  Input and Output simmilar to GET_TIME (A653)                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */



void w_GET_TIME(
  SYSTEM_TIME_TYPE *SYSTEM_TIME,
  RETURN_CODE_TYPE *RETURN_CODE)  {

  #ifdef DEBUG
  printf("w_GET_TIME\n");
  #endif

  GET_TIME( SYSTEM_TIME, RETURN_CODE);


}





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
  char wMessage[256] = "";

  #ifdef DEBUG  
  printf("w_SEND_QUEUING_MESSAGE %s (%d)\n", Message, Length);
  #endif
  
  /* execute service */
  SEND_QUEUING_MESSAGE( PortId, Message, Length, TimeOut, RetCode );


  // /* Display trace */
  // if( TraceActive == AVT_ACTIVE )
  // {
  //   /* validate Message */
  //   memset( wMessage, '\0', sizeof( wMessage ));
  //   ValidateString( Message, wMessage, Length);

  //   /* Trace init */
  //   DisplayHeader( "INIT", "SEND_QUEUING_MESSAGE" );
  //   printf(
  //      "QUEUING_PORT_ID : %ld \n"
  //      "%68s MESSAGE         : %s  \n"
  //      "%68s MESSAGE_SIZE    : %ld \n"
  //      "%68s TIME_OUT        : %lld )\n",
  //      PortId,
  //      "", wMessage,
  //      "", Length,
  //      "", TimeOut );
    
  //   /* Trace end */
  //   DisplayHeader( "END ", "SEND_QUEUING_MESSAGE" );
  //   printf(
  //      "RETURN_CODE     : %s )\n",
  //      TabError[*RetCode] );
  // }
}  /* w_SEND_QUEUING_MESSAGE */


