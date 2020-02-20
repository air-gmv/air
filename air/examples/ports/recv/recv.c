/*
 * Copyright (C) 2008-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
 
#include <rtems.h>
 
#include <rtems/rtems/tasks.h> 
#include <rtems/rtems/sem.h> 
#include <rtems/rtems/clock.h> 

#include <imaspex.h>

#ifdef RTEMS48I
	#include <printf.h>
#endif


#define TPS 200 /*ticks per second specified in the XML*/


SAMPLING_PORT_ID_TYPE RECV_PORT;

/*---------------------------------------------------------	
 *		function: test										*
 *			prints a simple string received via sampling	*
------------------------------------------------------------*/


void test(PARTITION_ID_TYPE self_id) {

	char message[1024];
	RETURN_CODE_TYPE rc;
	MESSAGE_SIZE_TYPE SIZE;
	
	

	/*Reference and returned timestamps for READ_SAMPLING_MESSAGE_CONDITIONAL*/
	SYSTEM_TIME_TYPE REF_TIME_STAMP=3000000000;
	SYSTEM_TIME_TYPE TIME_STAMP=0;
	
	while(1) {
		
		
		printf ("Partition %d receiving message..\n", self_id);
		
		/*if (TIME_STAMP > REF_TIME_STAMP), receive from sampling port*/
		READ_SAMPLING_MESSAGE_CONDITIONAL(RECV_PORT, REF_TIME_STAMP, (MESSAGE_ADDR_TYPE)message, &SIZE, &TIME_STAMP, &rc);
		if(INVALID_PARAM == rc) {
			printf("READ_SAMPLING_MESSAGE_CONDITIONAL error %d\n", rc);
		}
		else if((NO_ACTION == rc) && (0 == TIME_STAMP) && (0 == SIZE)) {
			printf("Empty Sampling Port\n");
		}
		else if((TIME_STAMP <= REF_TIME_STAMP) && (NO_ACTION == rc) && (0 == SIZE)) {
			printf("Timestamp not yet reached\n");
		}
		else if(TIME_STAMP > REF_TIME_STAMP) {
			printf("Message: %s, timestamp=%ldms\n", message, (long int)(TIME_STAMP/1000000) );
		}
		else {
			printf("Unexpected READ_SAMPLING_MESSAGE_CONDITIONAL behavior\n");
		}
			
			
		
		
		rtems_task_wake_after(0.6*TPS);  /*0.6 seconds guarantees it shoudl wake during the next execution window*/
	}
}


int entry_func() {


	RETURN_CODE_TYPE rc;

	rtems_name 	name 		= 1000;
	rtems_mode	mode		= RTEMS_PREEMPT ;
	rtems_mode	mode_mask	= RTEMS_PREEMPT_MASK;
	rtems_id	id;
	
	PARTITION_ID_TYPE self_id;
	
	
	/*Getting my own ID*/
	GET_PARTITION_ID(&self_id, &rc);
	if(NO_ERROR != rc) {
		printf("GET_PARTITION_ID error %d\n", rc);
	}
	
	printf("Initializing partition %d...\n", self_id);
	
	/*creating Destination sampling Port*/
	SAMPLING_PORT_NAME_TYPE NAME = "RECV_SAMP";
	MESSAGE_SIZE_TYPE SIZE = 1024;
	SYSTEM_TIME_TYPE PERIOD= 1500000000;
	
	
	CREATE_SAMPLING_PORT (NAME, SIZE, DESTINATION, PERIOD, &RECV_PORT, &rc);
	if (NO_ERROR != rc) {
		printf("CREATE_SAMPLING_PORT error %d\n", rc);
	}
	
	
	
	if (RTEMS_SUCCESSFUL == rtems_task_create (name, 15, 4096, mode, mode_mask, &id)) {
		rtems_task_start (id, (rtems_task_entry)test, self_id);
	}
	
	SET_PARTITION_MODE(NORMAL, &rc);
	if (NO_ERROR != rc) {
		printf("SET_PARTITION_MODE error %d\n", rc);
	}
	
	return RTEMS_SUCCESSFUL;
}
