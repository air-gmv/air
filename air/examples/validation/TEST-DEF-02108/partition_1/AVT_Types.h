/** 
 * @file AVT_Types.h
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  Essential type definitions, mostly inherited from AVT,
 *		or following the same primitives for future re-implementation
 * 
 */


#ifndef _AVT_TYPES_H
#define _AVT_TYPES_H

#ifndef _AVT_TYPES_H_
#define _AVT_TYPES_H_

#include <rtems.h>
#include <a653.h>

/*Strucutre defining Requirement condition validations referenced 
		in A653-Part3 for MMS
*/

typedef struct Matching_Error_Bits_s{

	unsigned int Cond1; 
	unsigned int Cond2; 
	unsigned int Cond3; 
	unsigned int Cond4; 
	unsigned int Cond5; 
	unsigned int Cond6; 
	unsigned int Cond7; 
	unsigned int Cond8;
	
} Matching_Error_Bits;

typedef struct Get_Schedule_Errors_s {
	unsigned TIME_OF_LAST_SCHEDULE_SWITCH : 1;
	unsigned CURRENT_SCHEDULE : 1;
	unsigned NEXT_SCHEDULE : 1;
	unsigned GET_MODULE_SCHEDULE_STATUS : 1;
	unsigned SET_MODULE_SCHEDULE : 1;
	unsigned TIMED_WAIT : 1;
	unsigned GET_TIME : 1;
	unsigned UNUSED : 25;
} Get_Schedule_Errors;


typedef enum 
{
	NOT_PERFORMED = 0,
	FAILED = 1,
	SUCCESS = 2
} TEST_RESULT_TYPE;

typedef rtems_boolean BOOLEAN;




/*PROCESS TYPES*/


typedef enum {
	DORMANT = 0,
	READY = 1,
	RUNNING = 2,
	WAITING = 3
} PROCESS_STATE_TYPE;

typedef enum { SOFT = 0, HARD = 1 } DEADLINE_TYPE;



typedef struct {
	SYSTEM_TIME_TYPE PERIOD;
	SYSTEM_TIME_TYPE TIME_CAPACITY;
	SYSTEM_ADDRESS_TYPE ENTRY_POINT;
	STACK_SIZE_TYPE STACK_SIZE;
	PRIORITY_TYPE BASE_PRIORITY;
	DEADLINE_TYPE DEADLINE;
	PROCESS_NAME_TYPE NAME;
} PROCESS_ATTRIBUTE_TYPE;




#endif
