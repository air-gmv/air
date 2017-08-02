/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2013
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
 
#include <rtems.h>
 
#include <rtems/rtems/tasks.h> 
#include <rtems/rtems/sem.h> 
#include <rtems/rtems/clock.h> 
 
#include <a653.h>
#include <imaspex.h>
 
#include <pprintf.h>

/*
 * gmvs
 * Garbage due to lazyness
 *
 * */

void append_to_message(uint8_t *msg_ptr_, char * to_append, int offset){

	int i;
	//strlen
	for( i = 0; to_append[i] != '\0'; i++);
	int length = i;


	for(i = 0; i < length; i++){
		msg_ptr_[offset+i] = to_append[i];
	}
	msg_ptr_[offset + length] = '\0';
}

void append_time_to_message(uint8_t *msg_ptr_, rtems_interval time, int offset){

	int i, length = 8;
	char time_tag[] = "00000000";
	for(i = length-1; time > 0 && i > 0; i--){
		int digit = time%10;
		time_tag[i] = digit + '0';
		time /= 10;
	}
	for(i = 0; i < length; i++){
		msg_ptr_[offset+i] = time_tag[i];
	}
	msg_ptr_[offset + length] = '\0';
}



/*
 * end of garbage
 * */



SAMPLING_PORT_ID_TYPE SEND_PORT;

/*---------------------------------------------------------	
 *		function: test										*
 *			outputs a simple string via a sampling port		* 
------------------------------------------------------------*/

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
	}
}


void test(PARTITION_ID_TYPE self_id) {

    int i = 0, offset = 0;;
	char message[256];
	char sample[3] = "S0 ";

	/* get the number of ticks per second */
	int tps = 1000000 / xky_syscall_get_us_per_tick();
	pprintf("TPS %i\n", tps);

	RETURN_CODE_TYPE rc = NO_ERROR;
	rtems_interval time;

	while(1) {

		rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &time);
		append_to_message(message, sample, offset);
		append_time_to_message(message, time, 3 + offset);
		append_to_message(message, " ", offset + 3 + 8);

		pprintf ("Partition %d at time %d sending: %s\n", self_id, time, message);
		WRITE_SAMPLING_MESSAGE (SEND_PORT, (MESSAGE_ADDR_TYPE )message, 1024, &rc );
		if (NO_ERROR != rc) {
			//pprintf("WRITE_SAMPLING_MESSAGE error %d\n", rc);
			error_message(rc);
		}
		offset += 12;
		/*identify the string with an integer index*/
		i++;
		if (i == 10) {
			i=0;
		}
		sample[1] = 0x30 + i;
		
		rtems_task_wake_after(0.7 * tps);
	}
}

int entry_func() {

	RETURN_CODE_TYPE rc;

	rtems_name 	name 		= 10000;
	rtems_mode	mode		= RTEMS_PREEMPT ;
	rtems_mode	mode_mask	= RTEMS_PREEMPT_MASK;
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
	
	
	if (RTEMS_SUCCESSFUL == rtems_task_create (name, 15, 4096, mode, mode_mask, &id)) {
		rtems_task_start (id, test, self_id);
	}
	
	SET_PARTITION_MODE(NORMAL, &rc);
	if (NO_ERROR != rc) {
		pprintf("SET_PARTITION_MODE error %d\n", rc);
	}
	
	return RTEMS_SUCCESSFUL;
}
