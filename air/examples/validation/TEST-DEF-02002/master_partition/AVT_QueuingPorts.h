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
 * | FILE:  AVT_QueuingPorts.h                                          | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVT_QUEUINGPORTS                                        | *
 * | UNIT NAME: AVT                                                     | *
 * |                                                                    | *
 * | DESCRIPTION: Header file with functions prototypes, definitions    | *
 * |              concern to QueuingPorts.c source.                     | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:    /  /                                                      | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/
#ifndef _AVT_QUEUING_PORTS_H_
#define _AVT_QUEUING_PORTS_H_

#include "AVT_TestTool.h"


/* Struct to store the errors generated by */
/* "CheckCreateQueuingPort" function  */
typedef struct
{
  unsigned ReturnCode     : 1;
  unsigned GetStatus      : 1;
  unsigned MaxMessageSize : 1;
  unsigned MaxNBMessage   : 1;
  unsigned PortDirection  : 1;
} CREATE_QUEUING_PORT_ERROR;


/* Struct to store the errors generated by */
/* "CheckSendQueuingMessage" and "CheckGetQueuingPortId" */
/* function  */

typedef struct
{
  unsigned ReturnCode    : 1;
  unsigned GetStatus     : 1;
} SEND_QUEUING_MESSAGE_ERROR,
  GET_QUEUING_PORT_ID_ERROR;

/* Struct to store the errors generated by */
/* "CheckReceiveQueuingMessage" function */
typedef struct
{
  unsigned ReturnCode    : 1;
  unsigned MessageSize   : 1;
  unsigned Message       : 1; 
  unsigned GetStatus     : 1;
} RECEIVE_QUEUING_MESSAGE_ERROR;


/* Struct to store the errors generated by */
/* "CheckGetQueuingPortSTATUS" function  */
typedef struct
{
  unsigned ReturnCode    : 1;
} GET_QUEUING_PORT_STATUS_ERROR;


/********************************/
/* functions module prototypes  */
/********************************/
CREATE_QUEUING_PORT_ERROR
CheckCreateQueuingPort( QUEUING_PORT_NAME_TYPE    InPortName,
			MESSAGE_SIZE_TYPE         InMaxMessageSize,
			MESSAGE_RANGE_TYPE        InMaxNBMessage,
			PORT_DIRECTION_TYPE       InPortDirection,
			QUEUING_DISCIPLINE_TYPE   InPortDiscipline,
			RETURN_CODE_TYPE          InReturnCode,
			QUEUING_PORT_ID_TYPE     *OutPortId,
			QUEUING_PORT_STATUS_TYPE *OutStatus,
			RETURN_CODE_TYPE         *OutReturnCode );

SEND_QUEUING_MESSAGE_ERROR
CheckSendQueuingMessage( QUEUING_PORT_ID_TYPE      InPortId,
			 APEX_BYTE                *InMessage,
			 MESSAGE_SIZE_TYPE         InMessageSize,
			 SYSTEM_TIME_TYPE          InTimeOut,
			 RETURN_CODE_TYPE          InReturnCode,
			 QUEUING_PORT_STATUS_TYPE *OutStatus,
			 RETURN_CODE_TYPE         *OutReturnCode );

RECEIVE_QUEUING_MESSAGE_ERROR
CheckReceiveQueuingMessage( QUEUING_PORT_ID_TYPE      InPortId,
			    SYSTEM_TIME_TYPE          InTimeOut,
			    APEX_BYTE                *InMessage,
			    MESSAGE_SIZE_TYPE         InMessageLength,
			    RETURN_CODE_TYPE          InReturnCode,
			    APEX_BYTE                *OutMessage,
			    MESSAGE_SIZE_TYPE        *OutMessageLength,
			    QUEUING_PORT_STATUS_TYPE *OutStatus,
			    RETURN_CODE_TYPE         *OutReturnCode );

GET_QUEUING_PORT_ID_ERROR
CheckGetQueuingPortId( QUEUING_PORT_NAME_TYPE    InPortName,
		       RETURN_CODE_TYPE          InReturnCode,
		       QUEUING_PORT_ID_TYPE     *OutPortId,
		       QUEUING_PORT_STATUS_TYPE *OutStatus,
		       RETURN_CODE_TYPE         *OutReturnCode );

GET_QUEUING_PORT_STATUS_ERROR
CheckQueuingPortStatus( QUEUING_PORT_ID_TYPE      InPortId,
			RETURN_CODE_TYPE          InReturnCode,
			QUEUING_PORT_STATUS_TYPE *OutStatus,
			RETURN_CODE_TYPE         *OutReturnCode );

#endif
/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |                   END OF FILE: AVT_QueuingPorts                    | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
