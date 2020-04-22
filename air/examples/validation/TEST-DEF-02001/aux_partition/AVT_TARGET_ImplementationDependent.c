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
 * | FILE: AVT_TARGET_ImplementationDependentDefinitions.c              | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVT_IMPLEMENTATION_DEPENDENT_DEFINITIONS                | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): CheckThatPartitionIsIdle                    | *
 * |                        CheckProcessCreationWithInvalidParameters   | *
 * |                        CheckSystemTimeClockValueOnStart            | *
 * |                                                                    | *
 * | DESCRIPTION: Tools for debug in development/maintenance            | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    19/04/2005                                                | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "AVT_TARGET_ImplementationDependent.h"

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: GetInvalidEnumeratedValue                           | *
 * |                                                                    | *
 * | DESCRIPTION:  To be used whenever a invalid enumerated value is    | *
 * |               needed                                               | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                        x                           | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE: IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE          | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidEnumeratedValue ( int *value )
{
  *value = INVALID_ENUMERATED_VALUE;
  return DEPENDENCE_AVAILABLE;
} /* GetInvalidEnumeratedValue */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: GetInvalidTimeValue                                 | *
 * |                                                                    | *
 * | DESCRIPTION:  To be used whenever a invalid time value is needed   | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                        x                           | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE: IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE          | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidTimeValue ( SYSTEM_TIME_TYPE *value )
{
  return DEPENDENCE_NOT_AVAILABLE;
} /* GetInvalidTimeValue */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: GetInvalidMemoryValue                               | *
 * |                                                                    | *
 * | DESCRIPTION: To be used whenever a invalid memeory value is needed | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                        x     Returned value        | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE: IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE          | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidMemoryValue ( SYSTEM_ADDRESS_TYPE *value )
{
  return DEPENDENCE_NOT_AVAILABLE;
} /* GetInvalidMemoryValue */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: GetInvalidStackValue                                | *
 * |                                                                    | *
 * | DESCRIPTION:  To be used whenever a invalid stack value is needed  | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                        x     Returned value        | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE: IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE          | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidStackValue ( STACK_SIZE_TYPE *value )
{
  return DEPENDENCE_NOT_AVAILABLE;
} /* GetInvalidStackValue */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: GetInvalidPriorityValue                             | *
 * |                                                                    | *
 * | DESCRIPTION:  To be used whenever a invalid priority value is      | *
 * |               needed                                               | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                        x     Returned value        | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE: IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE          | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidPriorityValue ( PRIORITY_TYPE *value )
{
  *value = INVALID_PRIORITY_VALUE;
  return DEPENDENCE_AVAILABLE;
} /* GetInvalidPriorityValue */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckThatPartitionIsIdle                            | *
 * |                                                                    | *
 * | DESCRIPTION:  To be used in test Test_PART_230_0020, test the      | *
 * |               service SET_PARTITION_MODE to IDLE.                  | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE: BOOLEAN FlagOK                                       | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE CheckThatPartitionIsIdle ( BOOLEAN *value )
{	
  return DEPENDENCE_NOT_AVAILABLE;
} /* CheckThatPartitionIsIdle */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckDeadlineValueAfterStart                        | *
 * |                                                                    | *
 * | DESCRIPTION: To be used in all START and DELAYED_START functional  | *
 * |              test procedures done in normal mode                   | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE: BOOLEAN FlagOK                                       | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE CheckDeadlineValueAfterStart ( SYSTEM_TIME_TYPE TimeBeforeStart, 
                                                                           SYSTEM_TIME_TYPE DeadlineValue,
																		   SYSTEM_TIME_TYPE TimeCapacity,
																		   BOOLEAN *value )
{
  *value = FALSE;
  if ( TimeCapacity == INFINITE_TIME_VALUE )
  {
	  *value = TRUE;
  }
  else if ( ( DeadlineValue >= TimeBeforeStart + TimeCapacity ) &&
		    ( DeadlineValue <= TimeBeforeStart + TimeCapacity + LAG_BETWEEN_CALLS ) )
  {
     *value = TRUE;	  
  }
  
  return DEPENDENCE_NOT_AVAILABLE;
} /* CheckDeadlineValueAfterStart */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: CheckDeadlineValueAfterStartBeforeNormalMode        | *
 * |                                                                    | *
 * | DESCRIPTION: To be used in all START and DELAYED_START functional  | *
 * |              test procedures done before normal mode               | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE: BOOLEAN FlagOK                                       | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE CheckDeadlineValueAfterStartBeforeNormalMode ( SYSTEM_TIME_TYPE EntryTimeOnFirstProcess, 
                                                                                           SYSTEM_TIME_TYPE DeadlineValue,
																									                SYSTEM_TIME_TYPE TimeCapacity,
																									                BOOLEAN *value )
{
  *value = FALSE;
  if ( TimeCapacity == INFINITE_TIME_VALUE )
  {
	  *value = TRUE;
  }
  else if ( ( DeadlineValue <= EntryTimeOnFirstProcess + TimeCapacity ) &&
		      ( DeadlineValue >= EntryTimeOnFirstProcess + TimeCapacity - LAG_BETWEEN_NORMAL_MODE_AND_FIRST_PROCESS_RUNNING ) )
  {
     *value = TRUE;	  
  }
  
  return DEPENDENCE_NOT_AVAILABLE;
} /* CheckDeadlineValueAfterStartBeforeNormalMode */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: DoSamplingPortExaustion                             | *
 * |                                                                    | *
 * | DESCRIPTION: To be used in the Test_INTER_520_0012, sampling port  | *
 * |              creation error handler                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE: BOOLEAN FlagOK                                       | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE DoSamplingPortExaustion ( BOOLEAN *value )
{
  unsigned int             i = 0;
  unsigned int             UsedSamplingPorts = NUMBER_USED_SAMPLING_PORTS;
  RETURN_CODE_TYPE         RetCode   = NO_ERROR;
  SAMPLING_PORT_NAME_TYPE  PortName;
  PORT_DIRECTION_TYPE      PortDir;
  SAMPLING_PORT_ID_TYPE    PortId;
  
  GET_SAMPLING_PORT_ID ( "SP_CREATE_TEST", &PortId, &RetCode );
  if ( RetCode != NO_ERROR )
  {
     w_CREATE_SAMPLING_PORT ( "SP_CREATE_TEST",
	  						  SAMPLING_PORT_MAX_MESSAGE_SIZE,
		                      SOURCE,
		                      SAMPLING_PORT_REFRESH_TIME,
							  &PortId,
							  &RetCode );
    EvaluateError ( RetCode, value  );
  }
  UsedSamplingPorts = UsedSamplingPorts + 1;

  char id[10];
  for( i = UsedSamplingPorts; i < MAX_NUMBER_OF_SAMPLING_PORTS *2; i++ )
  {
    memset( PortName, '\0', sizeof(PortName ));
    memset(id, 0, 10);

    strcpy(PortName, "QP_MAIN_DUMB");
    int_to_char(i, id);
    strcat(PortName, id);


    //sprintf( PortName, "SP_MAIN_DUMB%d", i );

	 if ( i%2 == 0 )
	 {
	    PortDir = SOURCE ;
	 }
    else
	 {
       PortDir = DESTINATION;
	 }

	 w_CREATE_SAMPLING_PORT ( PortName,
			                    SAMPLING_PORT_MAX_MESSAGE_SIZE,
			                    PortDir,
			                    SAMPLING_PORT_REFRESH_TIME,
			                    &PortId,
			                    &RetCode );
    EvaluateError ( RetCode, value  );
  }

  return DEPENDENCE_AVAILABLE;
} /* DoSamplingPortExaustion */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: GetInvalidMessageSizeValue                          | *
 * |                                                                    | *
 * | DESCRIPTION:  To be used whenever a invalid message size value is  / *
 * /               needed                                               | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                        x     Returned value        | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE: IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE          | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidMessageSizeValue ( MESSAGE_SIZE_TYPE *value )
{
  *value = -1;
  return DEPENDENCE_AVAILABLE;
} /* GetInvalidMessageSizeValue */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: GetAboveLimitMessageSizeValue                       | *
 * |                                                                    | *
 * | DESCRIPTION:  To be used whenever a above limit message size value / *
 * /               is needed                                            | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                        x     Returned value        | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE: IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE          | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetAboveLimitMessageSizeValue ( MESSAGE_SIZE_TYPE *value )
{
  *value = SYSTEM_LIMIT_MESSAGE_SIZE + 1;
  return DEPENDENCE_AVAILABLE;
} /* GetInvalidMessageSizeValue */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: GetInvalidMessageRangeValue                         | *
 * |                                                                    | *
 * | DESCRIPTION:  To be used whenever a invalid message range value is / *
 * /               needed                                               | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                        x     Returned value        | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE: IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE          | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidMessageRangeValue ( MESSAGE_RANGE_TYPE *value )
{
  *value = -1;
  return DEPENDENCE_AVAILABLE;
} /* GetInvalidMessageRangeValue */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: GetAboveLimitMessageRangeValue                      | *
 * |                                                                    | *
 * | DESCRIPTION:  To be used whenever a above limit message range      / *
 * /               value is needed                                      | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                        x     Returned value        | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE: IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE          | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetAboveLimitMessageRangeValue ( MESSAGE_RANGE_TYPE *value )
{
  *value = SYSTEM_LIMIT_NUMBER_OF_MESSAGES + 1;
  return DEPENDENCE_AVAILABLE;
} /*  GetAboveLimitMessageRangeValue */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: DoQueuingPortExaustion                              | *
 * |                                                                    | *
 * | DESCRIPTION: To be used in the Test_INTER_570_0012, queuing port   | *
 * |              creation error handler                                | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                                                    | *
 * | RETURN VALUE: BOOLEAN FlagOK                                       | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE DoQueuingPortExaustion ( BOOLEAN *value )
{
  unsigned int             i = 0;
  unsigned int             UsedQueuingPorts = NUMBER_USED_QUEUING_PORTS;
  RETURN_CODE_TYPE         RetCode   = NO_ERROR;
  QUEUING_PORT_NAME_TYPE   PortName;
  PORT_DIRECTION_TYPE      PortDir;
  QUEUING_PORT_ID_TYPE     PortId;
  
  printf("DoQueuingPortExaustion value: %d\n", *value);

  GET_QUEUING_PORT_ID ( "QP_CREATE_TEST", &PortId, &RetCode );
  if ( RetCode != NO_ERROR )
  {
	 w_CREATE_QUEUING_PORT ( "QP_CREATE_TEST",
	                         QUEUING_PORT_MAX_MESSAGE_SIZE,
	                         QUEUING_PORT_MAX_MESSAGE_NB,
	                          SOURCE,
	                          FIFO,
							  &PortId,
							  &RetCode );
	 EvaluateError ( RetCode, value  );
  }
  UsedQueuingPorts = UsedQueuingPorts + 1;

  printf("DoQueuingPortExaustion MAX_NUMBER_OF_QUEUING_PORTS = %d\n", MAX_NUMBER_OF_QUEUING_PORTS);
  char id[10];
  for( i = 0; i < MAX_NUMBER_OF_QUEUING_PORTS; i++ ){

	memset(id, 0, 10);
    memset( PortName, '\0', sizeof(PortName ));
    strcpy(PortName, "QP_MAIN_DUMB");
    int_to_char(i, id);
    strcat(PortName, id);

    //sprintf( PortName, "QP_MAIN_DUMB%d", i );

	 if ( i%2 == 1 )
	 {
	    PortDir = SOURCE;
	 }
    else
	 {
       PortDir = DESTINATION;
	 }

	w_CREATE_QUEUING_PORT( PortName,
			               QUEUING_PORT_MAX_MESSAGE_SIZE, 
			               QUEUING_PORT_MAX_MESSAGE_NB, 
			               PortDir, 
			               FIFO, 
			               &PortId, 
			               &RetCode );
    EvaluateError ( RetCode, value );
  }

  return DEPENDENCE_AVAILABLE;
} /* DoQueuingPortExaustion */





/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: ExaustStorageCapacity                               | *
 * |                                                                    | *
 * | DESCRIPTION:  To be used on CREATE_ERROR_HANDLER error handling    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                        x     Returned value        | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE: IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE          | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE ExaustStorageCapacity ( )
{
  return DEPENDENCE_NOT_AVAILABLE;
} /* ExaustStorageCapacity */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: GetInvalidErrorMessageSizeValue                     | *
 * |                                                                    | *
 * | DESCRIPTION:  To be used whenever a invalid error message size     / *
 * /               value is  needed                                     / *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                        x     Returned value        | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE: IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE          | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidErrorMessageSizeValue ( ERROR_MESSAGE_SIZE_TYPE *value )
{
  *value = -1;
  return DEPENDENCE_AVAILABLE;
} /* GetInvalidErrorMessageSizeValue */

/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: GetAboveLimitErrorMessageSizeValue                  | *
 * |                                                                    | *
 * | DESCRIPTION:  To be used whenever a above limit error message size / *
 * /               value is needed                                      / *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   19/04/2005                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |                                        x     Returned value        | *
 * |                                                                    | *
 * |                                                                    | *
 * | RETURN VALUE: IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE          | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetAboveLimitErrorMessageSizeValue ( ERROR_MESSAGE_SIZE_TYPE *value )
{
  *value = MAX_ERROR_MESSAGE_SIZE + 1;
  return DEPENDENCE_AVAILABLE;
} /* GetAboveLimitErrorMessageSizeValue */



/*
void START_SIMULATOR( SYSTEM_ADDRESS_TYPE *INITIALARRAY);
void StartPartition ( void );
void AuxPartitionMain( void );
*/
/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: main                                                | *
 * |                                                                    | *
 * | DESCRIPTION: Init simulation with desired partitions               | *
 * /                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   21/10/2004                                                 | *
 * |                                                                    | *
 * | SRS REQUIREMENT(S) ID:                                             | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
/*
int main ()
{
  SYSTEM_ADDRESS_TYPE functionStart[2];

	functionStart[0] = StartPartition;
	functionStart[1] = AuxPartitionMain;	
   START_SIMULATOR(functionStart);
   return 0;
}
*/


/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |   END OF FILE: AVT_TARGET_ImplementationDependentDefinitions.c     | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */

