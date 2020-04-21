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
 * | FILE: AVT_Wrapper.h                                                | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVTFUNCS                                                | *
 * | UNIT NAME: AVT                                                     | *
 * |                                                                    | *
 * | DESCRIPTION: Header file with functions prototypes, definitions    | *
 * |              concern to AVTWrapper.c source.                       | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   25/09/2002                                                 | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#ifndef _AVT_WRAPPER_H_
#define _AVT_WRAPPER_H_


#include "AVT_TestTool.h"
#include "AVT_LoadTables.h"

/* Time menegement */

void w_TIMED_WAIT( SYSTEM_TIME_TYPE  Delay,  RETURN_CODE_TYPE *RetCode );
void w_PERIODIC_WAIT( RETURN_CODE_TYPE *RetCode );
void w_GET_TIME( SYSTEM_TIME_TYPE *SysTime, RETURN_CODE_TYPE *RetCode );

void w_REPLENISH( SYSTEM_TIME_TYPE BudgetTime, RETURN_CODE_TYPE *RetCode );


/* process */
void w_SET_PRIORITY( PROCESS_ID_TYPE   PrcId,
		     PRIORITY_TYPE     Priority,
		     RETURN_CODE_TYPE  *RetCode );
void w_SUSPEND_SELF ( SYSTEM_TIME_TYPE  Delay, RETURN_CODE_TYPE *RetCode );    
void w_SUSPEND( PROCESS_ID_TYPE  PrcId, RETURN_CODE_TYPE *RetCode );    
void w_RESUME( PROCESS_ID_TYPE  PrcId,  RETURN_CODE_TYPE *RetCode );
void w_STOP_SELF( PROCESS_ID_TYPE  PrcId );
void w_STOP( PROCESS_ID_TYPE  PrcId, RETURN_CODE_TYPE *RetCode );
void w_START( PROCESS_ID_TYPE  PrcId, RETURN_CODE_TYPE *RetCode );
void w_DELAYED_START( PROCESS_ID_TYPE   PrcId,
		      SYSTEM_TIME_TYPE  Delay,
		      RETURN_CODE_TYPE *RetCode);
void w_LOCK_PREEMPTION( LOCK_LEVEL_TYPE *Level, RETURN_CODE_TYPE *RetCode );
void w_UNLOCK_PREEMPTION( LOCK_LEVEL_TYPE *Level, RETURN_CODE_TYPE *RetCode );
void w_GET_MY_ID( PROCESS_ID_TYPE  *PrcId, RETURN_CODE_TYPE *RetCode );
/*void w_GET_PROCESS_ID( PROCESS_NAME_TYPE  PrcName,
		       PROCESS_ID_TYPE   *PrcId,
		       RETURN_CODE_TYPE  *RetCode );
void w_GET_PROCESS_STATUS( PROCESS_ID_TYPE      PrcId,
			   PROCESS_STATUS_TYPE *PrcStatus,
			   RETURN_CODE_TYPE    *RetCode );*/

/* partitions */


void w_GET_PARTITION_STATUS( PARTITION_STATUS_TYPE *PrcStatus,
			     RETURN_CODE_TYPE      *RetCode );
void w_SET_PARTITION_MODE( OPERATING_MODE_TYPE OperMode,
			   RETURN_CODE_TYPE    *RetCode );

/* sampling ports */


void w_CREATE_SAMPLING_PORT( SAMPLING_PORT_NAME_TYPE Name,
			     MESSAGE_SIZE_TYPE       Length,
			     PORT_DIRECTION_TYPE     Direction,
			     SYSTEM_TIME_TYPE        Refresh,
			     SAMPLING_PORT_ID_TYPE  *PortId,
			     RETURN_CODE_TYPE       *RetCode );
void w_WRITE_SAMPLING_MESSAGE( SAMPLING_PORT_ID_TYPE PortId,
			       MESSAGE_ADDR_TYPE     Message,
			       MESSAGE_SIZE_TYPE     Length,
			       RETURN_CODE_TYPE      *RetCode );

void w_READ_SAMPLING_MESSAGE( SAMPLING_PORT_ID_TYPE PortId,
			      MESSAGE_ADDR_TYPE     Message,
			      MESSAGE_SIZE_TYPE    *Length,
			      VALIDITY_TYPE        *Validity,
			      RETURN_CODE_TYPE     *RetCode);
void w_GET_SAMPLING_PORT_ID( SAMPLING_PORT_NAME_TYPE PortName,
			     SAMPLING_PORT_ID_TYPE   *PortId,
			     RETURN_CODE_TYPE        *RetCode);
void w_GET_SAMPLING_PORT_STATUS( SAMPLING_PORT_ID_TYPE      PortId,
				 SAMPLING_PORT_STATUS_TYPE  *PortStatus,
				 RETURN_CODE_TYPE           *RetCode );

/* queuing ports */

void w_CREATE_QUEUING_PORT( QUEUING_PORT_NAME_TYPE  PortName,
			    MESSAGE_SIZE_TYPE       Length,
			    MESSAGE_RANGE_TYPE      Range,
			    PORT_DIRECTION_TYPE     Direction,
			    QUEUING_DISCIPLINE_TYPE Discipline,
			    QUEUING_PORT_ID_TYPE    *PortId,
			    RETURN_CODE_TYPE        *RetCode );
void w_SEND_QUEUING_MESSAGE( QUEUING_PORT_ID_TYPE PortId,
			     MESSAGE_ADDR_TYPE    Message,
			     MESSAGE_SIZE_TYPE    Length,
			     SYSTEM_TIME_TYPE     TimeOut,
			     RETURN_CODE_TYPE    *RetCode );
void w_RECEIVE_QUEUING_MESSAGE( QUEUING_PORT_ID_TYPE PortId,
				SYSTEM_TIME_TYPE     TimeOut,
				MESSAGE_ADDR_TYPE    Message,
				MESSAGE_SIZE_TYPE   *Length,
				RETURN_CODE_TYPE    *RetCode);
void w_GET_QUEUING_PORT_ID( QUEUING_PORT_NAME_TYPE PortName,
                            QUEUING_PORT_ID_TYPE   *PortId,
                            RETURN_CODE_TYPE       *RetCode );
void w_GET_QUEUING_PORT_STATUS( QUEUING_PORT_ID_TYPE     PortId,
                                QUEUING_PORT_STATUS_TYPE *PortStatus,
                                RETURN_CODE_TYPE         *RetCode );

/* buffers */


/*void w_CREATE_BUFFER( BUFFER_NAME_TYPE  BufName,
		      MESSAGE_SIZE_TYPE       Length,
		      MESSAGE_RANGE_TYPE      Range,
		      QUEUING_DISCIPLINE_TYPE Discipline,
		      BUFFER_ID_TYPE          *BufId,
		      RETURN_CODE_TYPE        *RetCode );
void w_SEND_BUFFER( BUFFER_ID_TYPE     BufId,
		    MESSAGE_ADDR_TYPE  Message,
		    MESSAGE_SIZE_TYPE  Length,
		    SYSTEM_TIME_TYPE   TimeOut,
		    RETURN_CODE_TYPE   *RetCode );
void w_RECEIVE_BUFFER( BUFFER_ID_TYPE     BufId,
		       SYSTEM_TIME_TYPE   TimeOut,
		       MESSAGE_ADDR_TYPE  Message,
		       MESSAGE_SIZE_TYPE  *Length,
		       RETURN_CODE_TYPE   *RetCode );
void w_GET_BUFFER_ID( BUFFER_NAME_TYPE BufName,
		      BUFFER_ID_TYPE   *BufId,
		      RETURN_CODE_TYPE *RetCode );
void w_GET_BUFFER_STATUS( BUFFER_ID_TYPE      BufId,
			  BUFFER_STATUS_TYPE  *BufStatus,
			  RETURN_CODE_TYPE    *RetCode );*/

/* blackboards */
/*
void w_CREATE_BLACKBOARD( BLACKBOARD_NAME_TYPE BlkName,
			  MESSAGE_SIZE_TYPE    Length,
			  BLACKBOARD_ID_TYPE  *BlkId,
			  RETURN_CODE_TYPE    *RetCode ); 
void w_DISPLAY_BLACKBOARD( BLACKBOARD_ID_TYPE BlkId,
			   MESSAGE_ADDR_TYPE  Message,
			   MESSAGE_SIZE_TYPE  Length,
			   RETURN_CODE_TYPE   *RetCode );
void w_READ_BLACKBOARD( BLACKBOARD_ID_TYPE BlkId,
			SYSTEM_TIME_TYPE   TimeOut,
			MESSAGE_ADDR_TYPE  Message,
			MESSAGE_SIZE_TYPE *Length,
			RETURN_CODE_TYPE  *RetCode );
void w_CLEAR_BLACKBOARD( BLACKBOARD_ID_TYPE  BlkId,
			 RETURN_CODE_TYPE    *RetCode );
void w_GET_BLACKBOARD_ID( BLACKBOARD_NAME_TYPE BlkName,
			  BLACKBOARD_ID_TYPE   *BlkId,
			  RETURN_CODE_TYPE     *RetCode );
void w_GET_BLACKBOARD_STATUS( BLACKBOARD_ID_TYPE     BlkId,
			      BLACKBOARD_STATUS_TYPE *BlkStatus,
			      RETURN_CODE_TYPE       *RetCode );
*/
/* semaphores */

/*
void w_CREATE_SEMAPHORE( SEMAPHORE_NAME_TYPE      SmfName,
			 SEMAPHORE_VALUE_TYPE     Value,
			 SEMAPHORE_VALUE_TYPE     MaxValue,
			 QUEUING_DISCIPLINE_TYPE  Discipline,
			 SEMAPHORE_ID_TYPE        *SmfId,
			 RETURN_CODE_TYPE         *RetCode );
void w_WAIT_SEMAPHORE( SEMAPHORE_ID_TYPE SmfId,
		       SYSTEM_TIME_TYPE  TimeOut,
		       RETURN_CODE_TYPE  *RetCode );
void w_SIGNAL_SEMAPHORE( SEMAPHORE_ID_TYPE  SmfId,
			 RETURN_CODE_TYPE   *RetCode );
void w_GET_SEMAPHORE_ID( SEMAPHORE_NAME_TYPE SmfName,
			 SEMAPHORE_ID_TYPE   *SmfId,
			 RETURN_CODE_TYPE    *RetCode );
void w_GET_SEMAPHORE_STATUS( SEMAPHORE_ID_TYPE      SmfId,
			     SEMAPHORE_STATUS_TYPE  *SmfStatus,
			     RETURN_CODE_TYPE       *RetCode );
*/
/* events */
/*
void w_CREATE_EVENT( EVENT_NAME_TYPE   EvtName,
		     EVENT_ID_TYPE    *EvtId,
		     RETURN_CODE_TYPE *RetCode );
void w_SET_EVENT( EVENT_ID_TYPE EvtId, RETURN_CODE_TYPE *RetCode );
void w_RESET_EVENT( EVENT_ID_TYPE EvtId, RETURN_CODE_TYPE *RetCode );
void w_WAIT_EVENT( EVENT_ID_TYPE    EvtId,
		   SYSTEM_TIME_TYPE TimeOut,
		   RETURN_CODE_TYPE *RetCode );
void w_GET_EVENT_ID( EVENT_NAME_TYPE   EvtName,
		     EVENT_ID_TYPE    *EvtId,
		     RETURN_CODE_TYPE *RetCode );
void w_GET_EVENT_STATUS( EVENT_ID_TYPE     EvtId,
			 EVENT_STATUS_TYPE *EvtStatus,
			 RETURN_CODE_TYPE  *RetCode );
*/
/* errors */


void
w_CREATE_ERROR_HANDLER( SYSTEM_ADDRESS_TYPE FuncName,
			STACK_SIZE_TYPE     Size,
			RETURN_CODE_TYPE   *RetCode );
void w_GET_ERROR_STATUS( ERROR_STATUS_TYPE *ErrStatus, 
			 RETURN_CODE_TYPE  *RetCode );
void w_RAISE_APPLICATION_ERROR( ERROR_CODE_TYPE    CodError,
				MESSAGE_ADDR_TYPE  Message,
				MESSAGE_SIZE_TYPE  Length,
				RETURN_CODE_TYPE  *RetCode );

void w_REPORT_APPLICATION_MESSAGE( MESSAGE_ADDR_TYPE  Message, 
			 MESSAGE_SIZE_TYPE    MessageSize,
			 RETURN_CODE_TYPE    *RetCode );

/* auxiliary functions */

void DisplayHeader( char *Type, char *Function );
void ValidateValue( long *Value );
/*void ValidateState( PROCESS_STATE_TYPE *Value );*/
void ValidateString( unsigned char *OldStr, char *NewStr, long Length );




#endif
/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                       END OF FILE: AVT_Wrapper.h                   | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
