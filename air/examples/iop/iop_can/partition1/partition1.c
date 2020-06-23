/*
 * Copyright (C) 2018-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * WARNING: When running this example on GR740, the following have to be configured:
 * - CAN pin multiplexing
 */

#include <rtems.h>
#include <string.h>

#include <a653.h>
#include <imaspex.h>

#include <pprintf.h>

SAMPLING_PORT_ID_TYPE SEND_PORT, SEND_PORT2;

SAMPLING_PORT_ID_TYPE RECEIVE_PORT, RECEIVE_PORT2;


void grcan_send_msg(PARTITION_ID_TYPE self_id){

	pprintf("This is Partition %d - Sender Task\n", self_id);
	char sample1[] = { 0xDE, 0xAD, 0xBE, 0xEF};
	char sample2[] = { 0xBE, 0xEF, 0xDE, 0xAD};

	int tps = 1000000 / air_syscall_get_us_per_tick();
	int sleep = 1*tps;
	pprintf("TPS %i\n", tps);

	pprintf("Sender task: Sleep for %ds so IOP gets ready...\n", sleep/1000);
	rtems_task_wake_after(sleep);
	pprintf("Sender task starting\n");
	RETURN_CODE_TYPE rc = NO_ERROR;

	while(1){
		pprintf("Sending %dbytes: 0xDEADBEEF\n", sizeof(sample1));
		WRITE_SAMPLING_MESSAGE(SEND_PORT,
				(MESSAGE_ADDR_TYPE) sample1,
				sizeof(sample1),
				&rc);

		pprintf("Sending %dbytes: 0xBEEFDEAD\n", sizeof(sample2));
		WRITE_SAMPLING_MESSAGE(SEND_PORT2,
				(MESSAGE_ADDR_TYPE) sample2,
				sizeof(sample2),
				&rc);

		rtems_task_wake_after(1* tps);
	}
}

void grcan_receive_msg(PARTITION_ID_TYPE self_id){

	pprintf("This is Partition %d - Receiver Task\n", self_id);
	char recv_msg[8] = {0,0,0,0,0,0,0,0};
	int tps = 1000000 / air_syscall_get_us_per_tick();
	int sleep = 2*tps;
	long int msg_len = 0;
	VALIDITY_TYPE validity;

	pprintf("TPS %i\n", tps);

	pprintf("Receiver task: Sleep for %ds so IOP gets ready...\n", sleep/tps);
	rtems_task_wake_after(sleep);
	pprintf("Receiver task starting\n");
	RETURN_CODE_TYPE rc = NO_ERROR;

	while(1){
		READ_SAMPLING_MESSAGE(RECEIVE_PORT,
				(MESSAGE_ADDR_TYPE) recv_msg,
				&msg_len,
				&validity,
				&rc);
		if(msg_len > 0)
		{
			pprintf("Partition%d received a message with length %d:\n", self_id, msg_len);
			for(int i = 0; i < msg_len; i++)
				pprintf("%x",recv_msg[i]);
			pprintf("\n");

			/* Clear buffer for next message */
			memset(recv_msg, 0, msg_len);
		}

		READ_SAMPLING_MESSAGE(RECEIVE_PORT2,
				(MESSAGE_ADDR_TYPE) recv_msg,
				&msg_len,
				&validity,
				&rc);
		if(msg_len > 0)
		{
			pprintf("Partition%d received a message with length %d:\n", self_id, msg_len);
			for(int i = 0; i < msg_len; i++)
				pprintf("%x",recv_msg[i]);
			pprintf("\n");

			/* Clear buffer for next message */
			memset(recv_msg, 0, msg_len);
		}

		rtems_task_wake_after(1*tps);
	}
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
	SAMPLING_PORT_NAME_TYPE RECEIVER_NAME2 = "partition_receiver2";
	SAMPLING_PORT_NAME_TYPE SENDER_NAME2 = "partition_sender2";
	MESSAGE_SIZE_TYPE SIZE = 8;
	SYSTEM_TIME_TYPE PERIOD= 1000000000ll;

	CREATE_SAMPLING_PORT (SENDER_NAME, SIZE, SOURCE, PERIOD, &SEND_PORT, &rc);
	if (NO_ERROR != rc) {
		pprintf("CREATE_SAMPLING_PORT %s, error: %d\n",SENDER_NAME, rc);
	}
	CREATE_SAMPLING_PORT (SENDER_NAME2, SIZE, SOURCE, PERIOD, &SEND_PORT2, &rc);
	if (NO_ERROR != rc) {
		pprintf("CREATE_SAMPLING_PORT %s, error: %d\n",SENDER_NAME, rc);
	}

	CREATE_SAMPLING_PORT (RECEIVER_NAME, SIZE, DESTINATION, PERIOD, &RECEIVE_PORT, &rc);
	if (NO_ERROR != rc) {
		pprintf("CREATE_SAMPLING_PORT %s, error: %d\n", RECEIVER_NAME, rc);
	}
	CREATE_SAMPLING_PORT (RECEIVER_NAME2, SIZE, DESTINATION, PERIOD, &RECEIVE_PORT2, &rc);
	if (NO_ERROR != rc) {
		pprintf("CREATE_SAMPLING_PORT %s, error: %d\n", RECEIVER_NAME, rc);
	}

	if (RTEMS_SUCCESSFUL == rtems_task_create (name_send, 15, 4096, mode, mode_mask, &id)) {
		rtems_task_start (id, (rtems_task_entry) grcan_send_msg, self_id);
	}
	if(RTEMS_SUCCESSFUL == rtems_task_create (name_receive, 25, 4096, mode, mode_mask, &id)){
		rtems_task_start(id, (rtems_task_entry) grcan_receive_msg, self_id);
	}

	SET_PARTITION_MODE(NORMAL, &rc);
	if (NO_ERROR != rc) {
		pprintf("SET_PARTITION_MODE error %d\n", rc);
	}

	return RTEMS_SUCCESSFUL;

}
