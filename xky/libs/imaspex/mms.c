/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
/** 
 * @file mms.c
 * @author sfbs
 * @brief ARINC 653 Part 2 Multiple Module Schedules: services interface
 */

#include <imaspex.h>

extern int imaspex_tsal_init;
extern xky_clocktick_t imaspex_ns_per_tick;

void SET_MODULE_SCHEDULE (
	/*in */ SCHEDULE_ID_TYPE SCHEDULE_ID,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE) {

    /* check TSAL init */
    if (imaspex_tsal_init == 0){
        *RETURN_CODE = INVALID_MODE;
        return;
    }
	
    /* sets next module schedule to schedule_id via a system call*/
    *RETURN_CODE = xky_syscall_set_schedule(SCHEDULE_ID);
}

void GET_MODULE_SCHEDULE_STATUS (
	/*out*/ SCHEDULE_STATUS_TYPE *SCHEDULE_STATUS,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE) {
	
    /* check TSAL init */
    if (imaspex_tsal_init == 0){
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get the schedule status via system call */
    xky_schedule_status_t status;
    *RETURN_CODE = xky_syscall_get_schedule_status(&status);

    /* convert status to ARINC status format */
    if (*RETURN_CODE == NO_ERROR) {
        SCHEDULE_STATUS->TIME_OF_LAST_SCHEDULE_SWITCH =
                status.time_of_last_schedule_change * imaspex_ns_per_tick;
        SCHEDULE_STATUS->CURRENT_SCHEDULE = status.current_schedule_id;
        SCHEDULE_STATUS->NEXT_SCHEDULE = status.next_schedule_id;
    }
}

void GET_MODULE_SCHEDULE_ID (
	/*in */ SCHEDULE_NAME_TYPE SCHEDULE_NAME,
	/*out*/ SCHEDULE_ID_TYPE *SCHEDULE_ID,
	/*out*/ RETURN_CODE_TYPE *RETURN_CODE) {

    /* check TSAL init */
    if (imaspex_tsal_init == 0){
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get the schedule id via system call */
    *RETURN_CODE = xky_syscall_get_schedule_id(SCHEDULE_NAME, SCHEDULE_ID);
}
