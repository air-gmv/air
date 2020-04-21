/** 
 * @file AVT_dependent.c
 * 
 *  COPYRIGHT (c) 2013
 *  GMV-SKYSOFT 
 *
 * @author Bruno Oliveira
 * @brief  AVT functions equivalent implementations and missing ARINC 
 *    functions workaround. Target dependent.
 * @note The functions contained in this file should be compatible with 
 *    other AVT implementations, i.e., this functions should substitute 
 *		other implementations and make it possible to integrate partition
 *		code in different environments.
 * 
 */


#include "AVT_dependent.h"

/*Ticks per second, configuration defined */
int ModuleTicksPerSecond = 1000;



/* Function Prototypes */


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_TIMED_WAIT					                              | *
 * |                                                                    | *
 * | DESCRIPTION: Adaptation of A653-P1 TIMED_WAIT for specific target  | *
 * |                                                                    | *
 * | AUTHOR: Bruno Oliveira                                             | *
 * | DATE:   20/09/2013                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   													            | *
 * | Like TIMED_WAIT                                                    | * 
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

void w_TIMED_WAIT ( APEX_LONG_INTEGER NanoSeconds , RETURN_CODE_TYPE *RetCode) {

  APEX_LONG_INTEGER SleepTicks = 
    NanoSeconds * ModuleTicksPerSecond / 1000000000ll;

  #ifdef DEBUG
  //printf("TIMED_WAIT %d ticks\n", (int) SleepTicks);
  #endif
  
  rtems_task_wake_after ( (rtems_interval) SleepTicks);
  *RetCode = NO_ERROR;
}


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_STOP_SELF						                              | *
 * |                                                                    | *
 * | DESCRIPTION: Adaptation of A653-P1 STOP_SELF for specific target   | *
 * |                                                                    | *
 * | AUTHOR: Bruno Oliveira                                             | *
 * | DATE:   20/09/2013                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   													            | *
 * | Like TIMED_WAIT                                                    | *  
 * |                                                                    | *
 * ---------------------------------------------------------------------- */

void w_STOP_SELF( void ) {
  pmk_system_halt();
}


/* ---------------------------------------------------------------------- +
 * |                                                                    | *
 * | FUNCTION NAME: w_CREATE_PROCESS						                        | *
 * |                                                                    | *
 * | DESCRIPTION: Adaptation of A653-P1 CREATE_PROCESS for 							| *
 * |    specific target  																							  | *
 * |                                                                    | *
 * | AUTHOR: Bruno Oliveira                                             | *
 * | DATE:   20/09/2012                                                 | *
 * |                                                                    | *
 * | PARAMETERS:                   													            | *
 * | Like CREATE_PROCESS                                                | *  
 * |                                                                    | *
 * ---------------------------------------------------------------------- */


void w_CREATE_PROCESS( 
	PROCESS_ATTRIBUTE_TYPE *ATTRIBUTES, /*in*/
	PROCESS_ID_TYPE *PROCESS_ID, /*out*/
	RETURN_CODE_TYPE *RETURN_CODE /*out*/ 
	) {

	#ifdef DEBUG
		printf("w_CREATE_PROCESS\n");
	#endif

	rtems_status_code rc = 0;
	rtems_name pname = 0;
	rtems_id pid;
	int i;

	if (strlen(ATTRIBUTES->NAME) >= 4) {
		pname = rtems_build_name(
			ATTRIBUTES->NAME[0],
			ATTRIBUTES->NAME[1],
			ATTRIBUTES->NAME[2],
			ATTRIBUTES->NAME[3]);
	} else {
		#ifdef DEBUG
			printf("w_CREATE_PROCESS invalid name length %d\n", strlen(ATTRIBUTES->NAME));
		#endif
		*RETURN_CODE = INVALID_PARAM;
		return;
	}
	
	/*ToDo: ATTRIBUTES verification */

	rc = rtems_task_create(
		pname,
		(rtems_task_priority) ATTRIBUTES->BASE_PRIORITY,
		(size_t)ATTRIBUTES->STACK_SIZE,
		(RTEMS_PREEMPT | RTEMS_NO_TIMESLICE ),
		(RTEMS_FLOATING_POINT | RTEMS_LOCAL),
		&pid
		);

	if (rc != RTEMS_SUCCESSFUL) {
		#ifdef DEBUG
			printf("CREATE_PROCESS rtems_task_create rc = %d\n", (int)rc);
		#endif
		*RETURN_CODE = NO_ACTION;
		return;
	}

	rc = rtems_task_start(pid, (rtems_task_entry) ATTRIBUTES->ENTRY_POINT,
		NULL);

	if (rc != RTEMS_SUCCESSFUL) {
		#ifdef DEBUG
			printf("CREATE_PROCESS rtems_task_start rc = %d\n", (int)rc);
		#endif
		*RETURN_CODE = NO_ACTION;
		return;
	}

	*PROCESS_ID = (PROCESS_ID_TYPE) pid;
	*RETURN_CODE = NO_ERROR;

}







extern SYSTEM_TIME_TYPE Start_System_Time = 0;
extern SYSTEM_TIME_TYPE Current_Period_Start = 0;


void CalcPeriodStart (void) {

  RETURN_CODE_TYPE RetCode;
  SYSTEM_TIME_TYPE PrevTime = 0ll;
  SYSTEM_TIME_TYPE CurrTime = 0ll;

  while(1) {
    //w_GET_TIME(&PrevTime, &RetCode);
    PrevTime = (SYSTEM_TIME_TYPE)((pmk_get_current_clock_ticks() + 1)*1000000ll);
    w_TIMED_WAIT(1000000ll, &RetCode);
    //w_GET_TIME(&CurrTime, &RetCode);
    CurrTime = (SYSTEM_TIME_TYPE)((pmk_get_current_clock_ticks() + 1)*1000000ll);

    if (CurrTime - PrevTime > (SYSTEM_TIME_TYPE)2000000ll) {
      #ifdef DEBUG
        printf("dif %ld CalcPeriodStart at %ld\n", 
          (long)(CurrTime - PrevTime),
          (long)PrevTime);
      #endif DEBUG
      Current_Period_Start = PrevTime;
    } 
  }

}



void EmptyReturnStruct(Get_Schedule_Errors *ReturnStruct) {

  ReturnStruct->TIME_OF_LAST_SCHEDULE_SWITCH = (unsigned)0;
  ReturnStruct->CURRENT_SCHEDULE = (unsigned)0;
  ReturnStruct->NEXT_SCHEDULE = (unsigned)0;
  ReturnStruct->GET_MODULE_SCHEDULE_STATUS = (unsigned)0;
  ReturnStruct->SET_MODULE_SCHEDULE = (unsigned)0;
  ReturnStruct->TIMED_WAIT = (unsigned)0;
  ReturnStruct->GET_TIME = (unsigned)0;
  ReturnStruct->UNUSED = 0;
}