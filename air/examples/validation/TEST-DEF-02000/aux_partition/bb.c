/*
 * write_bb.c
 *
 *  Created on: Jul 5, 2012
 *      Author: cassia
 */
#include <rtems.h>
#include <pal_printf.h>
#include <a653.h>
#include "bb.h"
#include "AVT_TestTool.h"

char bb_message[30];
int bb_message_len;
rtems_id bb_id;



int write_bb( char * message, size_t len){

	rtems_status_code rtems_rc;

	memset(bb_message, 0, 30);
	strncpy(bb_message, message, len);
	bb_message_len = len;

	printf("unblocking\n");
	rtems_rc = rtems_semaphore_release(bb_id);
	if (rtems_rc != RTEMS_SUCCESSFUL){
		printf("rtems_semaphore_release failed\n");
	}

	return rtems_rc;
}


int read_bb(char * o_msg){
	rtems_status_code rtems_rc;
	rtems_interval rtems_timeout;

	rtems_timeout = RTEMS_NO_TIMEOUT;

	//if((strncmp(bb_message, "SubBBAuxId", 10)) != 0){
	if(bb_message_len <= 0){
		printf("blocking\n");
		rtems_rc = rtems_semaphore_obtain(bb_id, RTEMS_WAIT, rtems_timeout);
	}else{
		strcpy(*o_msg, bb_message);

	}

	return 0;
}


int create_bb(){

	rtems_status_code rtems_rc;

	memset(bb_message, 0, 30);

	rtems_rc = rtems_semaphore_create(
				"sem",
				0,
				(RTEMS_SIMPLE_BINARY_SEMAPHORE |  RTEMS_FIFO ),
				0,
				&bb_id);

	bb_message_len = 0;
	return rtems_rc;
}
