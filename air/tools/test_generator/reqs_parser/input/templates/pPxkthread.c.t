/* RTEMS Partition Kernel Thread (User module) */

#include <rtems.h>
#include <pmk_linkcmd_vars.h>

extern rtems_configuration_table Configuration;

extern void (*_RTEMS_tasks_Initialize_user_tasks_p)(void);

rtems_id				kthread;


rtems_status_code resume_kthread (int i,void *p) {
	if (kthread) {
	return rtems_task_resume(kthread);
	} else {
		return -1;
	}
}

rtems_status_code set_kthread_id (int i,void *p) {
	kthread = (rtems_id) i;
	return RTEMS_SUCCESSFUL;
}

int get_configuration(void) {
	return (int) &Configuration;
}

int get_initialize_user_tasks_p(void){
	return (int) _RTEMS_tasks_Initialize_user_tasks_p;
}
