
/* ***********************************************************************	*/
/* ** TEST-DEF-80300 / 80300 ******************************************** */
/* ***********************************************************************	*/

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>

/* Test Printf *******************************************************	*/
int test_printf (void) {
	
	while(1) {
		pal_pprintf_process();
		/* TODO : this value is somewhat random */
		rtems_task_wake_after(10);
	}	
	return 0;
}