/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2013
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
 
#include <rtems.h>

#include <a653.h>
#include <imaspex.h>
 
#include <pprintf.h>

SAMPLING_PORT_ID_TYPE SEND_PORT;

void error_message(RETURN_CODE_TYPE rc){

	switch(rc){
		case INVALID_CONFIG:
			pprintf("WRITE_SAMPLING_MESSAGE error Invalid Config\n");
		break;
		case INVALID_PARAM:
			pprintf("WRITE_SAMPLING_MESSAGE error Invalid Param\n");
		break;
		case INVALID_MODE:
			pprintf("WRITE_SAMPLING_MESSAGE error Invalid Mode\n");
		break;
		default:
			pprintf("WRITE_SAMPLING_MESSAGE error %d\n", rc);
		break;
	}
}

/*---------------------------------------------------------	
 *		function: test										*
 *			outputs a simple string via a sampling port		* 
------------------------------------------------------------*/
void test(PARTITION_ID_TYPE self_id) {

	char sample[3] = "S0 ";
	/* get the number of ticks per second */
	uint32_t tps = 1000000 / air_syscall_get_us_per_tick();
	pprintf("TPS %i\n", tps);
    uint32_t interval = 0;

	RETURN_CODE_TYPE rc = NO_ERROR;
	rtems_interval time;

	while(1) {

#ifdef RTEMS48I
		rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &time);
#else
		time = rtems_clock_get_ticks_since_boot();
#endif
		pprintf ("Partition %d at time %d sending: %s\n", self_id, time, sample);

		WRITE_SAMPLING_MESSAGE (SEND_PORT, (MESSAGE_ADDR_TYPE )sample, 3, &rc );
		if (NO_ERROR != rc)
			error_message(rc);

		/*identify the string with an integer index*/
		sample[1] += 1;

		if (sample[1] == ':')
			sample[1] = '0';

#ifdef RTEMS48I
		// 0.7 * tps does not work!
		interval = tps * 7;
		interval /= 10;
#else
		interval = tps * 0.7;
#endif
		pprintf("interval %i\n", interval);

		rtems_task_wake_after(interval);
	}
}

int entry_func() {

	RETURN_CODE_TYPE rc;

	rtems_name 	name 		= 10000;
	rtems_mode	mode		= RTEMS_PREEMPT ;
#ifdef RTEMS48I
	rtems_mode	mode_mask		= RTEMS_PREEMPT_MASK;
#else
	rtems_attribute	attribute_set	= RTEMS_LOCAL | RTEMS_FLOATING_POINT;
#endif
	rtems_id	id;
	
	PARTITION_ID_TYPE self_id;
	
	/*Getting my own partition id*/
	GET_PARTITION_ID(&self_id, &rc);
	if(NO_ERROR != rc) {
		pprintf("GET_PARTITION_ID error %d\n", rc);
	}

	pprintf("Initializing partition %d...\n", self_id);
	
	/*Creating Source sampling Port*/
	SAMPLING_PORT_NAME_TYPE NAME = "ssampling";
	MESSAGE_SIZE_TYPE SIZE = 1024;
	SYSTEM_TIME_TYPE PERIOD= 1000000000ll;
	
	CREATE_SAMPLING_PORT (NAME, SIZE, SOURCE, PERIOD, &SEND_PORT, &rc);
	if (NO_ERROR != rc) {
		pprintf("CREATE_SAMPLING_PORT error %d\n", rc);
	}
#ifdef RTEMS48I
	if (RTEMS_SUCCESSFUL == rtems_task_create (name, 15, 4096, mode, mode_mask, &id)) {
		rtems_task_start (id, test, self_id);
	}
#else
	if (RTEMS_SUCCESSFUL == rtems_task_create (name, 15, 4096, mode, attribute_set, &id)) {
		rtems_task_start (id, (rtems_task_entry)test, self_id);
	}
#endif
	SET_PARTITION_MODE(NORMAL, &rc);
	if (NO_ERROR != rc) {
		pprintf("SET_PARTITION_MODE error %d\n", rc);
	}
	
	return RTEMS_SUCCESSFUL;
}
