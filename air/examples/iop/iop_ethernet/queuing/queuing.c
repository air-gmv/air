/*
 * Copyright (C) 2008-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */
 
#include <rtems.h>
 
#include <rtems/rtems/tasks.h> 
#include <rtems/rtems/sem.h> 
#include <rtems/rtems/clock.h> 
 
#include <a653.h>
#include <imaspex.h>
 
#include <pprintf.h>


QUEUING_PORT_ID_TYPE qpid;


/*---------------------------------------------------------	
 *		function: test										*
 *			prints a simple string 							*
------------------------------------------------------------*/


void test(PARTITION_ID_TYPE self_id) {
	unsigned char msg[1024]="empty\0";
	RETURN_CODE_TYPE rc;
	MESSAGE_SIZE_TYPE len;
    pprintf ("Ready to receive\n", self_id, msg);
	while(1) {

		RECEIVE_QUEUING_MESSAGE(qpid, INFINITE_TIME_VALUE, msg, &len, &rc );
		if (rc == NO_ERROR)
		    pprintf ("Partition %d: %s\n", self_id, msg);
		else
			if(rc == INVALID_CONFIG)
				pprintf ("Partition %d Overflow: %s\n", self_id, msg);
	}
}


int entry_func() {


	RETURN_CODE_TYPE rc;

	rtems_name 	name 		= 	2000;
	rtems_mode	mode		= RTEMS_PREEMPT ;
	rtems_mode	mode_mask	= RTEMS_PREEMPT_MASK;
	rtems_id	id;
	
	PARTITION_ID_TYPE self_id;

	CREATE_QUEUING_PORT("dqueuing", 1024, 32, DESTINATION, FIFO, &qpid, &rc );
	if(NO_ERROR != rc){
		pprintf("CREATE_QUEUING_PORT error %d\n", rc);
	} 
	
	GET_PARTITION_ID(&self_id, &rc);
	if(NO_ERROR != rc){
		pprintf("GET_PARTITION_ID error %d\n", rc);
	}
	pprintf("Initializing partition %d...\n", self_id);
	
	if (RTEMS_SUCCESSFUL == rtems_task_create (name, 15, 4096, mode, mode_mask, &id)) {
		rtems_task_start (id, (rtems_task_entry)test, self_id);
	}
	
	SET_PARTITION_MODE(NORMAL, &rc);
	if (NO_ERROR != rc){
			pprintf("SET_PARTITION_MODE error %d\n", rc);
	}

	return RTEMS_SUCCESSFUL;
}
