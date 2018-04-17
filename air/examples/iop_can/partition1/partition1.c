/*
 * partition1.c
 *
 *  Created on: Mar 15, 2018
 *      Author: gmvs
 *
 * NOTE: When running this example on GR740, the following have to be configured:
 * - CAN pin multiplexing
 * - Enable GRCAN clock
 *
 * This can be done in GRMON2 using the following commands before running the
 * example:
 *   grmon2> wmem 0xffa0b000 0x000ffc3c
 *   grmon2> grcg enable 5
 *
 */

#include <rtems.h>

#include <rtems/rtems/tasks.h>
#include <rtems/rtems/sem.h>
#include <rtems/rtems/clock.h>

#include <a653.h>
#include <imaspex.h>

#include <pprintf.h>

SAMPLING_PORT_ID_TYPE SEND_PORT;

SAMPLING_PORT_ID_TYPE RECEIVE_PORT;


void grcan_send_msg(PARTITION_ID_TYPE self_id){

	pprintf("This is Partition %d - Sender Task\n", self_id);
	int i = 0;
	char sample1[] = "GMV";
	char sample2[] = "GMVS";
	char sample3[] = "TEST0";

	int tps = 1000000 / air_syscall_get_us_per_tick();
	int sleep = 1*tps;
	pprintf("TPS %i\n", tps);

	pprintf("Partition%d: Sleep for %ds so IOP gets ready...\n", self_id, sleep/1000);
	rtems_task_wake_after(sleep);
	pprintf("Partition%d: Starting ", self_id);
	RETURN_CODE_TYPE rc = NO_ERROR;
	rtems_interval time;

	while(1){
		pprintf("Sending message ..%s.. through CAN\n", sample1);
		WRITE_SAMPLING_MESSAGE(SEND_PORT, (MESSAGE_ADDR_TYPE) sample1, 3, &rc);

//		pprintf("Sending message ..%s.. through CAN\n", sample2);
//		WRITE_SAMPLING_MESSAGE(SEND_PORT, (MESSAGE_ADDR_TYPE) sample2, 4, &rc);
//
//		pprintf("Sending message ..%s.. through CAN\n", sample3);
//		WRITE_SAMPLING_MESSAGE(SEND_PORT, (MESSAGE_ADDR_TYPE) sample3, 5, &rc);
		i++;
		if(i == 10){
			i = 0;
		}
		sample3[4] = 0x30 + i;

		rtems_task_wake_after(1* tps);
	}
}

void grcan_receive_msg(PARTITION_ID_TYPE self_id){

}

int entry_func(){

	RETURN_CODE_TYPE rc;

	rtems_name name_send = 71771;
	rtems_name name_receive = 71772;
	rtems_mode mode = RTEMS_PREEMPT;
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
	SAMPLING_PORT_NAME_TYPE RECEIVER_NAME = "partition_receiver";
	SAMPLING_PORT_NAME_TYPE SENDER_NAME = "partition_sender";
	MESSAGE_SIZE_TYPE SIZE = 8;
	SYSTEM_TIME_TYPE PERIOD= 1000000000ll;

	CREATE_SAMPLING_PORT (SENDER_NAME, SIZE, SOURCE, PERIOD, &SEND_PORT, &rc);
	if (NO_ERROR != rc) {
		pprintf("CREATE_SAMPLING_PORT %s, error: %d\n",SENDER_NAME, rc);
	}
	pprintf("CREATE_SAMPLING_PORT %s\n", SENDER_NAME);
	CREATE_SAMPLING_PORT (RECEIVER_NAME, SIZE, DESTINATION, PERIOD, &RECEIVE_PORT, &rc);
	if (NO_ERROR != rc) {
		pprintf("CREATE_SAMPLING_PORT %s, error: %d\n", RECEIVER_NAME, rc);
	}
	pprintf("CREATE_SAMPLING_PORT %s\n", RECEIVER_NAME);

	if (RTEMS_SUCCESSFUL == rtems_task_create (name_send, 15, 4096, mode, mode_mask, &id)) {
		rtems_task_start (id, grcan_send_msg, self_id);
	}

//	if (RTEMS_SUCCESSFUL == rtems_task_create (name_receive, 15, 4096, mode, mode_mask, &id)) {
//		rtems_task_start (id, test, self_id);
//	}

	SET_PARTITION_MODE(NORMAL, &rc);
	if (NO_ERROR != rc) {
		pprintf("SET_PARTITION_MODE error %d\n", rc);
	}

	return RTEMS_SUCCESSFUL;

}
