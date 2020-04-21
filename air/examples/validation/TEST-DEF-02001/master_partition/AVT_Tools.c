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
 * | FILE: AVT_Tools.c                                                  | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVTTOOLS                                                | *
 * | UNIT NAME: AVT                                                     | *
 * | CONTAINED FUNCTION(S): DisplayQPStatus                             | *
 * |                        DisplayPartitionStatus                      | *
 * |                        DisplayProcessStatus                        | *
 * |                        DisplayStr                                  | *
 * |                        DisplayNum                                  | *
 * |                        DisplayError                                | *
 * |                        DisplayStatusSemaphore                      | *
 * |                        DisplayBufferStatus                         | *
 * |                                                                    | *
 * | DESCRIPTION: Tools for debug in development/maintenance            | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    25/09/2002...                                             | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#include "AVT_Funcs.h"

//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: DisplayQPStatus                                     | *
 * |                                                                    | *
 * | DESCRIPTION: Dis+lay Queuing Ports Status                          | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   18/10/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * |  QUEUING_PORT_NAME_TYPE *QPName X            Port name             | *
 * |                                                                    | *
 * | RETURN VALUE: RETURN_CODE_TYPE RetCode                             | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE
DisplayQPStatus ( QUEUING_PORT_NAME_TYPE QPName )
{
  RETURN_CODE_TYPE         RetCode = NO_ERROR;
  QUEUING_PORT_ID_TYPE     PortId  = 0;
  QUEUING_PORT_STATUS_TYPE PortStatus;

  GET_QUEUING_PORT_ID( QPName, &PortId, &RetCode );

  if ( RetCode == NO_ERROR )
  {
     GET_QUEUING_PORT_STATUS( PortId, &PortStatus, &RetCode );
  }
  
  if ( RetCode == NO_ERROR )
  {
    printf ("\nPort NAME        %s\n"
	    "NB_MESSAGE         %ld\n"
	    "MAX_NB_MESSAGE     %ld\n"
	    "MAX_MESSAGE_SIZE   %ld\n"
	    "PORT_DIRECTION     %s\n"
	    "WAITING_PROCESSES  %ld\n",
	    QPName,
	    PortStatus.NB_MESSAGE,
	    PortStatus.MAX_NB_MESSAGE,
	    PortStatus.MAX_MESSAGE_SIZE,
	    (( PortStatus.PORT_DIRECTION == 0) ? "SOURCE" : "DESTINATION" ),
	    PortStatus.WAITING_PROCESSES );
  }
  return RetCode;

} /* DisplayQPStatus */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: DisplayPartitionStatus                              | *
 * |                                                                    | *
 * | DESCRIPTION: Display partition status                              | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Desiption             | *
 * |                                                                    | *
 * | RETURN VALUE: RETURN_CODE_TYPE RetCode                             | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
RETURN_CODE_TYPE
DisplayPartitionStatus( void )
{
  RETURN_CODE_TYPE       RetCode = NO_ERROR;
  PARTITION_STATUS_TYPE  Status;

  GET_PARTITION_STATUS ( &Status, &RetCode ); 

  printf ( "IDENTIFIER      %ld\n"
	  "PERIOD          %lld\n"
	  "DURATION        %lld\n"
	  "LOCK_LEVEL      %d\n"
	  "OPERATING_MODE  %d\n",
	  Status.IDENTIFIER,
	  Status.PERIOD,
	  Status.DURATION,
	  (unsigned int)Status.LOCK_LEVEL,
	  (unsigned int)Status.OPERATING_MODE );

  return RetCode;

} /* DisplayPartitionStatus */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: DisplayStr                                          | *
 * |                                                                    | *
 * | DESCRIPTION: Display two strings                                   | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | char *str                       X            1� displayed str      | *
 * | char *Message                   X            2 � displayed str     | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
DisplayStr( char *str, char *Message  )
{
  printf ("%s %s \n",
	 ((str == NULL) ? "-" : str),
  	 ((Message == NULL) ? "-" : Message));


} /* Display */



/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: DisplayError                                        | *
 * |                                                                    | *
 * | DESCRIPTION: Receive error code and display error ans description  | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   Input  Output  Description           | *
 * | char *str                              X     Error description     | *
 * | RETURN_CODE_TYPE RetCode        X            Code error            | *
 * |                                                                    | *
 * | RETURN VALUE:                                                      | *
 * |                                                                    | *
 * ---------------------------------------------------------------------- */
void
DisplayError(  char *str, RETURN_CODE_TYPE RetCode )
{
  char string[128];
  char *TabError[] = {"NO_ERROR","NO_ACTION","NOT_AVAILABLE","INVALID_PARAM",
		      "INVALID_CONFIG","INVALID_MODE","TIMED_OUT" };

  
  if( str == NULL )
  {
    //strcpy( string, "" );
	  memcpy(string, "" , 0);
  }
  else
  {
    //strcpy( string, str );
    memcpy(string, "" , 128);
  }

  printf ("%s- Error %ld - %s \n", string, (long)RetCode, TabError[RetCode] );

} /* DisplayError */











/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                    END OF FILE: AVT_Tools.c                        | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */

