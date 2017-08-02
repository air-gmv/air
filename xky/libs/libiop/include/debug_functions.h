/*
 * debug_functions.h
 *
 *  Created on: Jul 31, 2017
 *      Author: assert
 */

#ifndef LIBS_LIBIOP_INCLUDE_DEBUG_FUNCTIONS_H_
#define LIBS_LIBIOP_INCLUDE_DEBUG_FUNCTIONS_H_

#include <iop.h>

void append_to_message(uint8_t *msg_ptr, char * to_append, int offset);

void append_time_to_message(uint8_t *msg_ptr, rtems_interval time , int offset);

int home_made_strlen(char *str);


#endif /* LIBS_LIBIOP_INCLUDE_DEBUG_FUNCTIONS_C_ */
