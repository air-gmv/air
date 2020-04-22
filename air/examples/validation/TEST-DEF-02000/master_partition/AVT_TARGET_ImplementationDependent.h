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
 * | FILE: AVT_TARGET_ImplementationDependentDefinitions.h              | *
 * | VERSION: A653-1 R1.0                                               | *
 * | $Log$                                                              | *
 * |                                                                    | *
 * | CSCI NAME: AVT_IMPLEMENTATION_DEPENDENT_DEFINITIONS                | *
 * | UNIT NAME: AVT                                                     | *
 * |                                                                    | *
 * | DESCRIPTION: Header file with functions prototypes and definitions | *
 * |              that are considered to be implementation dependent    | *
 * |              and must be implemented by the provider of the target | *
 * |              system under test           .                         | *
 * |                                                                    | *
 * |                                                                    | *
 * | AUTHOR:                                                            | *
 * | DATE:   18/04/2005                                                 | *
 * |                                                                    | *
 * |                                                                    | *
 * +--------------------------------------------------------------------+ *
 *------------------------------------------------------------------------*/

#ifndef _AVT_IMPLEMENTATION_DEPENDENT_DEFINITIONS_H_
#define _AVT_IMPLEMENTATION_DEPENDENT_DEFINITIONS_H_

#include <limits.h>
#include <string.h>
#include "AVT_TARGET_Boolean.h"
//#include "a653.h"
#include "AVT_Wrapper.h"

/* These defines set the max and min values for all ARINC 653 numeric types 
   As defined they relate to environment machine limit values */

#define APEX_BYTE_MAX CHAR_MAX

#define APEX_BYTE_MIN CHAR_MIN

#define APEX_INTEGER_MAX LONG_MAX

#define APEX_INTEGER_MIN LONG_MIN

#define APEX_UNSIGNED_MAX ULONG_MAX

#define APEX_LONG_INTEGER_MAX 2*LONG_MAX*LONG_MAX

#define APEX_LONG_INTEGER_MIN -1*APEX_LONG_INTEGER_MAX

/* Must modify these values to values that are implementation dependent 
   Some of these values also concern performance */

/*set of defines for values regarding PARTITION period and duration*/
#define PART_PERIOD 400000

#define PART_DURATION 200000

/*set of defines to be issued on TIMED WAITS call on tests*/
#define TOLERANCE_VALUE 100000

#define CYCLE_DELAY 100000

#define SMALL_WAIT 50000

#define BIG_WAIT 5000000

#define REGULAR_TIMED_WAIT 1000000

#define BELOW_REGULAR_TIMED_WAIT REGULAR_TIMED_WAIT - TOLERANCE_VALUE

#define BELOW_TO_ABOVE_REGULAR_TIMED_WAIT 2*TOLERANCE_VALUE

#define ABOVE_REGULAR_TIMED_WAIT REGULAR_TIMED_WAIT + TOLERANCE_VALUE

//#define MAXIMUM_TIMEWAIT_FOR_RESTART 3000000
#define MAXIMUM_TIMEWAIT_FOR_RESTART 3000000000

/* The minimum time needed for a partition that was shutdown to restart */
#define MINIMUM_TIMEWAIT_FOR_RESTART 3000000

#define PARTITION_PERIOD 400000

#define PARTITION_EXECUTION_TIME 200000

#define PERIODIC_PROCESS_PERIOD 100000

#define QUEUING_TEST_WAIT 1000

#define SAFE_TIME_CAPACITY 100*PARTITION_EXECUTION_TIME

/*define value for a invalid enumerated value*/
#define INVALID_ENUMERATED_VALUE -1

/*define value for a invalid process id value*/
#define INVALID_PROCESS_ID_VALUE -1

/*define PERIOD atribbute of an aperiodico value*/
#define APERIODIC_PROCESS INFINITE_TIME_VALUE

/*??????????????????? */
/*#define MAX_TIME_VALUE    86400000*/
  #define BIG_STACK_SIZE 0x50000
  #define REGULAR_STACK_SIZE 0x5000
  #define SMALL_STACK_SIZE 0x80

/* We also use the following defines, leaving open the possibility for
   a different range of priority values */
#define MAX_PRIORITY_VALUE 63

#define MIN_PRIORITY_VALUE 1

#define INVALID_PRIORITY_VALUE -1

/* this are the standard prioritie used on the AVT_Tool processes*/
#define REGULAR_MASTER_PROCESS_PRIORITY 2
#define REGULAR_P1_PROCESS_PRIORITY 42
#define REGULAR_P2_PROCESS_PRIORITY 21
#define REGULAR_P100_PROCESS_PRIORITY 45
#define HIGH_PROCESS_PRIORITY 63

#define AUX_REGULAR_MASTER_PROCESS_PRIORITY 6
#define AUX_REGULAR_P1_PROCESS_PRIORITY 42

/*values used on the sampling ports used by the AVT Tool*/
//#define SAMPLING_PORT_REFRESH_TIME 1500
#define SAMPLING_PORT_REFRESH_TIME 400000000
#define SAMPLING_PORT_REFRESH_TICKS 400

#define SAMPLING_PORT_MAX_MESSAGE_SIZE 128

/*values used on the queuing ports used by the AVT Tool*/
#define QUEUING_PORT_MAX_MESSAGE_SIZE 64
#define QUEUING_PORT_MAX_MESSAGE_NB 8
#define QUEING_PORT_MAX_MESSAGE_NB_BOTH_ENDS QUEUING_PORT_MAX_MESSAGE_NB*2

/*if enumerated values size is above 16 bytes then this type must be changed*/
/*the queuing port using this constant is sendig binary numeric (under the form of enumerated) data*/
/*if enumerated is above 16 bytes then problems may exist*/
#define QUEUING_PORT_MAX_STARTUPINFO_SIZE 32 

/*values used on the buffer used by the AVT Tool*/
#define BUFFER_MAX_MESSAGE_SIZE 128
#define BUFFER_MAX_MESSAGE_NB 8

/*values used on the blackboard used by the AVT Tool*/
#define BLACKBOARD_MAX_MESSAGE_SIZE 128

/*values used on the semaphore used by the AVT Tool*/
#define SEMAPHORE_MAX_VALUE 5


/* this value is used when we need to get the time a service is called. We call before */
/*GET_TIME and this constant will denote the maximum time passed in between calls */
#define LAG_BETWEEN_CALLS 100

/* this value is used to determine the maximum time frame between the moment where the partition*/
/* is set to normal mode and starts executing its first process*/
#define LAG_BETWEEN_NORMAL_MODE_AND_FIRST_PROCESS_RUNNING 1000000

/*set of defines with the number of ressources used by AVT Tool */
/*used to ehxaust the resources */

#define NUMBER_USED_PROCESSES 4
#define NUMBER_USED_SAMPLING_PORTS 2
#define NUMBER_USED_QUEUING_PORTS 7
#define NUMBER_USED_BUFFERS 2
#define NUMBER_USED_BLACKBOARDS 3
#define NUMBER_USED_SEMAPHORES 3
#define NUMBER_USED_EVENTS 1

/*set of fucntions that execute, if available implementation dependent code*/
typedef enum 
{
	DEPENDENCE_NOT_AVAILABLE,
	DEPENDENCE_AVAILABLE
}  IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE;

/* to be used whenever a invalid enumerated value is needed */	
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidEnumeratedValue ( int *value );	

/* to be used whenever a invalid time value is needed */	
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidTimeValue ( SYSTEM_TIME_TYPE *value );	

/* to be used on tests where an invalid memory value is used*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidMemoryValue ( SYSTEM_ADDRESS_TYPE *value );

/* to be used on tests where an invalid stack value is needed*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidStackValue ( STACK_SIZE_TYPE *value );

/* to be used on tests where a invalid priority is needed*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidPriorityValue ( PRIORITY_TYPE *value );

/* To be used in test Test_PART_230_0020, test the service SET_PARTITION_MODE to IDLE. */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE CheckThatPartitionIsIdle( BOOLEAN *value );

/* To be used in all START and DELAYED_START functional test procedures when calls are */
/*done in normal mode*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE CheckDeadlineValueAfterStart( SYSTEM_TIME_TYPE TimeBeforeStart, 
                                                                          SYSTEM_TIME_TYPE DeadlineValue,
																		  SYSTEM_TIME_TYPE TimeCapacity,
																		  BOOLEAN *value);

/* To be used in all START and DELAYED_START functional test procedures when calls are */
/*done before normal mode*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE CheckDeadlineValueAfterStartBeforeNormalMode ( SYSTEM_TIME_TYPE EntryTimeOnFirstProcess, 
                                                                                           SYSTEM_TIME_TYPE DeadlineValue,
																							SYSTEM_TIME_TYPE TimeCapacity,
																							BOOLEAN	*value );
/* To be used in all periodic process START functional procedures */ 
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetFirstReleasePointAfterStart ( SYSTEM_TIME_TYPE InitialTime,
		                                                                       SYSTEM_TIME_TYPE *ReleasePoint );

/* to be used on tests where a invalid priority is needed*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidProcessIdValue ( PROCESS_ID_TYPE *value );

/*To be used on Test_PROC_230_0012 to test processes available resources*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE DoProcessExaustion ( BOOLEAN *value );

/*To be used on Test_INTER_520_0012 to test sampling port available resources*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE DoSamplingPortExaustion ( BOOLEAN *value );

/*To be used whenever a invalid message size value is needed */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidMessageSizeValue ( MESSAGE_SIZE_TYPE *value );

/*To be used whenever a above limit message size value is needed */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetAboveLimitMessageSizeValue ( MESSAGE_SIZE_TYPE *value);

/*To be used whenever a invalid message size value is needed */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidMessageRangeValue ( MESSAGE_RANGE_TYPE *value );

/*To be used whenever a above limit message range value is needed */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetAboveLimitMessageRangeValue ( MESSAGE_RANGE_TYPE *value );

/*To be used on Test_INTER_570_0012 to test queuing port available resources*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE DoQueuingPortExaustion ( BOOLEAN *value );

/*To be used on Test_INTRA_240_0012 to test buffer available resources*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE DoBufferExaustion ( BOOLEAN *value );

/*To be used on Test_INTRA_290_0012 to test blackboard available resources*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE DoBlackboardStorageExaustion ( RETURN_CODE_TYPE *RetCode );

/*To be used whenever a invalid semaphore value is needed */
/*IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidSemaphoreValue ( SEMAPHORE_VALUE_TYPE *value );*/

/*To be used whenever a above limit semaphore value is needed */
/*IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetAboveLimitSemaphoreValue ( SEMAPHORE_VALUE_TYPE *value );*/
		
/*To be used on Test_INTRA_350_0012 to test blackboard available resources*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE DoSemaphoreExaustion ( BOOLEAN *value );

/*To be used on Test_INTRA_400_0012 to test blackboard available resources*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE DoEventExaustion ( BOOLEAN *value );

/*To be used on Test_HMON_090_0012 to test error on creation of ERROR HANDLER*/
/*due to storage capacity being insuficient*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE ExaustStorageCapacity ( );
		
/*To be used whenever a invalid error message size value is needed */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetInvalidErrorMessageSizeValue ( ERROR_MESSAGE_SIZE_TYPE *value );

/*To be used whenever a above limit error message size value is needed */
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetAboveLimitErrorMessageSizeValue ( ERROR_MESSAGE_SIZE_TYPE *value );

/*To be used on test Test_HMON_120_0040 to check availability of HM recovery action*/		
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetHMTableErrorWithIdleRecoveryAvailability ( );

/*To be used on test Test_HMON_120_0040 to produce HM recovery action*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE ProduceHMTableErrorWithIdleRecovery ( );

/*To be used on test Test_HMON_120_0010 to check availability of HM recovery action*/		
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetHMTableErrorWithWarmStartRecoveryAvailability ( );

/*To be used on test Test_HMON_120_0010 to produce HM recovery action*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE ProduceHMTableErrorWithWarmStartRecovery ( );

/*To be used on test Test_HMON_120_0020 to check availability of HM recovery action*/		
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetHMTableErrorWithColdStartRecoveryAvailability ( );

/*To be used on test Test_HMON_120_0020 to produce HM recovery action*/
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE ProduceHMTableErrorWithColdStartRecovery ( );

/*To be used on test Test_HMON_120_0030 to check availability of HM recovery action*/		
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE GetHMTableErrorWithDoNothingRecoveryAvailability ( );

/*To be used on test Test_HMON_120_0030 to produce HM recovery action*/		
IMPLEMENTATION_DEPENDENCE_AVAILABILITY_TYPE ProduceHMTableErrorWithDoNothingRecovery ( );

#endif
/* +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ *
 * |                                                                    | *
 * |   END OF FILE: AVT_TARGET_ImplementationDependentDefinitions.h     | *
 * |                                                                    | *
 * +xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx+ */
