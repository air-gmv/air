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
  printf("TIMED_WAIT %d ticks\n", (int) SleepTicks);
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

