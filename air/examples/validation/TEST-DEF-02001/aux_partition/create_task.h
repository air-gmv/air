/*
 * create_task.h
 *
 *  Created on: Jul 5, 2012
 *      Author: cassia
 */
#include <rtems.h>

#ifndef CREATE_TASK_H_
#define CREATE_TASK_H_

typedef struct avt_ctx_st{
	char name[2];
	rtems_id id;
	void * entry;
	rtems_task_priority prio;
	struct avt_ctx_st * nxt;
}avt_ctx_t;

int avt_create_rtems_task(avt_ctx_t *p_ctx);

#endif /* CREATE_TASK_H_ */
