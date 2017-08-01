/*
 * debug_functions.h
 *
 *  Created on: Jul 31, 2017
 *      Author: gmvs
 */

#include <debug_functions.h>

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
	for(i = length; time > 0 && i > 0; i--){
		int digit = time%10;
		time_tag[i] = digit + '0';
		time /= 10;
	}
	for(i = 0; i < length; i++){
		msg_ptr_[offset+i] = time_tag[i];
	}
	msg_ptr_[offset + length] = '\0';
}
