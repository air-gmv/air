/*
 * create_task.c
 *
 *  Created on: Jul 5, 2012
 *      Author: cassia
 */

#include "create_task.h"
#include <pal_printf.h>
#include "AuxPartitionMain.h"
#define APX_STACK_SIZE 4*1024

int avt_create_rtems_task(avt_ctx_t *p_ctx){

	rtems_status_code 	rc;
	rtems_name 			tname = (rtems_name)p_ctx->name;
	size_t 				stacksize;
	rtems_id id = -1;

	stacksize = APX_STACK_SIZE;


 	/* AuxMasterProcess AUX_REGULAR_MASTER_PROCESS_PRIORITY */
 	rc = rtems_task_create(
			tname,												// 	name
			AUX_REGULAR_MASTER_PROCESS_PRIORITY,								//	prio
			stacksize,											//	stack size
			(RTEMS_PREEMPT | RTEMS_NO_TIMESLICE ),	//	init modes
			(RTEMS_FLOATING_POINT | RTEMS_LOCAL),				//	attribs
			(Objects_Id *)&id);//&(p_ctx->id));										//	fill tid

	if (rc != RTEMS_SUCCESSFUL){
		printf("Can't rtems_task_create | rtems_status_code=%d \n", rc);
		return rc;
	}

	p_ctx->id = id;

	printf("created task %d\n", id);

	/* SubAuxProcess AUX_REGULAR_P1_PROCESS_PRIORITY */
	rc = rtems_task_start(p_ctx->id, p_ctx->entry , (rtems_task_argument)p_ctx);
	if (rc != RTEMS_SUCCESSFUL){
		printf("failed to rtems_task_start taskid [%u] "
				"| rtems_status_code=%d \n", (unsigned)p_ctx->id, rc);
		return -10;
	}


	return NO_ERROR;
}


