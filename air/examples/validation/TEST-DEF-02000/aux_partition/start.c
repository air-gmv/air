/*
 * start.c
 *
 *  Created on: Jul 9, 2012
 *      Author: cassia
 */


#include <a653.h>
#include <pal_printf.h>
#include "start.h"

RETURN_CODE_TYPE start(unsigned char * i_name){

	avt_ctx_t * runner = tasks_ctx;
	rtems_status_code rc = -1;

	while(runner != NULL){
		if((strcmp((char *)i_name, runner->name)) == 0){

			rc = rtems_task_start(runner->id, runner->entry , (rtems_task_argument)runner);
			if (rc != RTEMS_SUCCESSFUL){
				printf("failed to rtems_task_start taskid [%u] "
						"| rtems_status_code=%d \n", (unsigned)runner->id, rc);
				return -10;
			}


			return NO_ERROR;
		}
	}
}
