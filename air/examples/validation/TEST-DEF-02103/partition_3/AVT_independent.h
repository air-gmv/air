/** 
 * @file AVT_independent.h
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  AVT original implementations function prototypes
 * @note The functions contained in this file should be compatible
 *    with any target or implementation supporting the ARINC-653 
 *    Specification.
 * 
 */
#ifndef _AVT_INDEPENDENT_H_
#define _AVT_INDEPENDENT_H_

#include "AVT_Types.h"
#include "AVT_dependent.h"
#include <a653.h>






/* Function Prototypes */

TEST_RESULT_TYPE EvaluateResult( BOOLEAN FlagOK );

void
w_SET_PARTITION_MODE( OPERATING_MODE_TYPE OperMode,
          RETURN_CODE_TYPE    *RetCode );


void print_test_results(TEST_RESULT_TYPE TestResult); 

void w_SET_MODULE_SCHEDULE(
  SCHEDULE_ID_TYPE SchedId,
  RETURN_CODE_TYPE *ReturnCode
  );

void w_GET_MODULE_SCHEDULE_STATUS(
  SCHEDULE_STATUS_TYPE *ScheduleStatus,
  RETURN_CODE_TYPE *RetCode) ;

void
w_GET_QUEUING_PORT_ID( QUEUING_PORT_NAME_TYPE PortName,
           QUEUING_PORT_ID_TYPE   *PortId,
           RETURN_CODE_TYPE        *RetCode );


void
w_RECEIVE_QUEUING_MESSAGE( 
         QUEUING_PORT_ID_TYPE PortId,
         SYSTEM_TIME_TYPE     TimeOut,
         MESSAGE_ADDR_TYPE    Message,
         MESSAGE_SIZE_TYPE   *Length,
         RETURN_CODE_TYPE    *RetCode );

void
w_CREATE_QUEUING_PORT( QUEUING_PORT_NAME_TYPE  PortName,
           MESSAGE_SIZE_TYPE       Length,
           MESSAGE_RANGE_TYPE      Range,
           PORT_DIRECTION_TYPE     Direction,
           QUEUING_DISCIPLINE_TYPE Discipline,
           QUEUING_PORT_ID_TYPE    *PortId,
           RETURN_CODE_TYPE        *RetCode );

void
w_GET_PARTITION_STATUS( PARTITION_STATUS_TYPE *PrtStatus,
      RETURN_CODE_TYPE      *RetCode );


void w_GET_TIME(
  SYSTEM_TIME_TYPE *SYSTEM_TIME,
  RETURN_CODE_TYPE *RETURN_CODE);


void
w_SEND_QUEUING_MESSAGE( QUEUING_PORT_ID_TYPE PortId,
      MESSAGE_ADDR_TYPE    Message,
      MESSAGE_SIZE_TYPE    Length,
      SYSTEM_TIME_TYPE     TimeOut,
      RETURN_CODE_TYPE    *RetCode );

#endif
