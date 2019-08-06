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
#include <air.h>

#ifdef RTEMS48I
	#include <printf.h>
#endif




#define TPS 100 /*ticks per second specified in the XML*/

SAMPLING_PORT_ID_TYPE SEND_PORT;
QUEUING_PORT_ID_TYPE qpid;

/*---------------------------------------------------------	
 *		function: test										*
 *			outputs a simple string via a sampling port		* 
------------------------------------------------------------*/

void test(PARTITION_ID_TYPE self_id) {

	char message[18]= "This is sample 0\0";
	int i=0;
	
	RETURN_CODE_TYPE rc;
	
	while(1) {
		printf ("Partition %d sending: %s..\n", self_id, message);
		WRITE_SAMPLING_MESSAGE (SEND_PORT, (MESSAGE_ADDR_TYPE )message, 16, &rc );
		if (NO_ERROR != rc) {
			printf("WRITE_SAMPLING_MESSAGE error %d\n", rc);
		}
		
				/*identify the string with an integer index*/
		i++;
		if (i == 10) {
			i=0;
		}
		message[15] = 0x30 + i;
		
    printf ("Partition %d sending queuing: %s..\n", self_id, message);
    SEND_QUEUING_MESSAGE(qpid, (MESSAGE_ADDR_TYPE )message, 16, INFINITE_TIME_VALUE, &rc );
		if (rc != NO_ERROR) {
		    printf ("SEND_QUEUING_MESSAGE error %d\n", rc);
		}
		
		/*identify the string with an integer index*/
		i++;
		if (i == 10) {
			i=0;
		}
		message[15] = 0x30 + i;
		
		
		rtems_task_wake_after(0.6*TPS); 
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
		printf("GET_PARTITION_ID error %d\n", rc);
	}
	
	printf("Initializing partition %d...\n", self_id);
	
	
	/*Creating Source sampling Port*/
	SAMPLING_PORT_NAME_TYPE NAME = "SEND_SAMP";
	MESSAGE_SIZE_TYPE SIZE = 1024;
	SYSTEM_TIME_TYPE PERIOD= 1500000000;
	
	CREATE_SAMPLING_PORT (NAME, SIZE, SOURCE, PERIOD, &SEND_PORT, &rc);
	if (NO_ERROR != rc) {
		printf("CREATE_SAMPLING_PORT error %d\n", rc);
	}
	
	/* Creating Queueing port */
  CREATE_QUEUING_PORT("QSAMPLE", 1024, 32, SOURCE, FIFO, &qpid, &rc );
	if(NO_ERROR != rc){
		printf("CREATE_QUEUING_PORT error %d\n", rc);
	} 
	
	
	if (RTEMS_SUCCESSFUL == rtems_task_create (name, 15, 4096, mode, mode_mask, &id)) {
		rtems_task_start (id, test, self_id);
	}
	
	SET_PARTITION_MODE(NORMAL, &rc);
	if (NO_ERROR != rc) {
		printf("SET_PARTITION_MODE error %d\n", rc);
	}
	
	return RTEMS_SUCCESSFUL;
}
