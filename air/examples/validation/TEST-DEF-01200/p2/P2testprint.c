
/* ***********************************************************************	*/
/* ** TEST-DEF-01200 / 1200 ********************************************* */
/* ***********************************************************************	*/

#include <rtems.h>

#include <air.h>
#include <air_test.h>

/* Test Printf *******************************************************	*/
int test_printf (void) {
	
	while(1) {
		control_partition_init(1, TEST_MEMSHARE);
		/* TODO : this value is somewhat random */
		rtems_task_wake_after(15);
	}	
	return 0;
}