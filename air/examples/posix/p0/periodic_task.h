/*
 * periodic_task.h: a factory of periodic task
 */

#ifndef __PERIODIC_TASK_H__
#define __PERIODIC_TASK_H__

typedef struct _task_parameters {
  struct timespec period; /* period of a task */
  void (*job)(void); /* parameter-less function, executed at each dispatch */
} task_parameters;

void create_periodic_task (struct timespec period, void (*job) (void));
/* Create a periodic task, of period 'period'. At each dispatch, it
 * will execute the job function
 */

#endif /* __PERIODIC_TASK_H__ */
