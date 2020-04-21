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

#endif /* CREATE_TASK_H_ */
