/*
 * Copyright (C) 2008-2019  GMVIS Skysoft S.A.
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


SAMPLING_PORT_ID_TYPE RECV_PORT2;
QUEUING_PORT_ID_TYPE qpid;

/*---------------------------------------------------------	
 *		function: test										*
 *			Gets the sampling port status.					* 
 *			If there is  anew message, read with 			*
 *			READ_UPDATED_SAMPLING_MESSAGE, else				*
 *			read with READ_SAMPLING_MESSAGE and				*
 *			check its validity								*
 *			RECEIVE_QUEUING_MESSAGE								*
------------------------------------------------------------*/

/*-----------------------------------------------------------
type SAMPLING_PORT_CURRENT_STATUS_TYPE is record
REFRESH_PERIOD : SYSTEM_TIME_TYPE;
TIME_STAMP : SYSTEM_TIME_TYPE;
MAX_MESSAGE_SIZE : MESSAGE_SIZE_TYPE;
PORT_DIRECTION : PORT_DIRECTION_TYPE;
MESSAGE_AGE : AGE_TYPE;
UPDATED : UPDATED_TYPE;
end record;
-----------------------------------------------------------*/

void test(uintptr_t self_id) {

	unsigned char message[1024];
	RETURN_CODE_TYPE rc;
	MESSAGE_SIZE_TYPE SIZE;
	UPDATED_TYPE UPDATED;
	VALIDITY_TYPE VALIDITY;
	SAMPLING_PORT_CURRENT_STATUS_TYPE STATUS;
 	MESSAGE_SIZE_TYPE len;
	
	while(1) {
		
		printf ("Partition %d receiving message..\n", self_id);
		
		
		GET_SAMPLING_PORT_CURRENT_STATUS(RECV_PORT2, &STATUS, &rc);
		if(NO_ERROR != rc) {
			printf("GET_SAMPLING_PORT_CURRENT_STATUS error %d\n", rc);
		}
		
		switch (STATUS.UPDATED) {
		
			case NEW_MESSAGE:
				READ_UPDATED_SAMPLING_MESSAGE(RECV_PORT2, (MESSAGE_ADDR_TYPE)message, &SIZE, &UPDATED, &rc);
				if(NO_ERROR != rc) {
					printf("READ_UPDATED_SAMPLING_MESSAGE error %d\n", rc);
				}
				
				printf("NEW_MSG: %s\n", message);
			break;
			
			case CONSUMED_MESSAGE:
				READ_SAMPLING_MESSAGE(RECV_PORT2, (MESSAGE_ADDR_TYPE)message, &SIZE, &VALIDITY, &rc);
				if(NO_ERROR != rc) {
					printf("READ_SAMPLING_MESSAGE error %d\n", rc);
				}
				
				switch (VALIDITY) {
					case INVALID: 
						printf("INVALID: %s\n", message);
					break;
					case VALID:
						printf("VALID: %s\n", message);
					break;
				}
			break;
			
			case EMPTY_PORT:
				message[0]='\0';
				printf("Empty sampling port\n");
			break;
			
			default:
				printf("Error in STATUS.UPDATED value\n");
			break;
		}

		RECEIVE_QUEUING_MESSAGE(qpid, INFINITE_TIME_VALUE, message, &len, &rc );
		if (rc == NO_ERROR) {
		    printf ("Received Partition Queue message %d: %s\n", self_id, message);
		}
		else
    {
        printf("Error in Receiving Queue Message - %d\n", rc);
    }
		rtems_task_wake_after(0.6*TPS); 
	}
}


int entry_func() {


	RETURN_CODE_TYPE rc;

	rtems_name 	name 		= 2000;
	rtems_mode	mode		= RTEMS_PREEMPT ;
	rtems_mode	mode_mask	= RTEMS_PREEMPT_MASK;
	rtems_id	id;
	
	PARTITION_ID_TYPE self_id;
	
	GET_PARTITION_ID(&self_id, &rc);
	if (NO_ERROR != rc) {
		printf("GET_PARTITION_ID error %d\n", rc);
	}
	
	printf("Initializing partition %d...\n", self_id);
	
	/*creating Destination sampling Port*/
	SAMPLING_PORT_NAME_TYPE NAME = "RECV_SAMP2";
	MESSAGE_SIZE_TYPE SIZE = 1024;
	SYSTEM_TIME_TYPE PERIOD= 1500000000;
	
	
	CREATE_SAMPLING_PORT (NAME, SIZE, DESTINATION, PERIOD, &RECV_PORT2, &rc);
	if (NO_ERROR != rc) {
		printf("CREATE_SAMPLING_PORT error %d\n", rc);
	}
	
		CREATE_QUEUING_PORT("QSAMPLE", 1024, 32, DESTINATION, FIFO, &qpid, &rc );
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
