
/* ***********************************************************************    */
/* ** TEST-DEF-00009 / 9 ************************************************ */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0120                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Call the air_syscall_set_schedule function from an authorized partition and
        check that the partitioning schedule is altered starting at the next major time
        frame (MTF) start. Check that a call to air_syscall_set_schedule with the current
        schedule produces no change. Check that the call from a non-authorized partition
        fails with an HM process/ partition event. */

#include <rtems.h>
#include <air.h>
#include <air_test.h>
#include <P0testdef.h>
#include <a653.h>
#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error = 0;
int mtf_ticks = 50;

/* Test auxiliary functions    ********************************************	*/

/**
 Call rtems clock get
 */
int _rtems_clock_get(rtems_interval *t1) {
#ifdef RTEMS48I
    rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, t1);
#else
    *t1 = rtems_clock_get_ticks_since_boot();
#endif
    return RTEMS_SUCCESSFUL;
}

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id, air_error_e i_error) {
    /* signal error ocurrence	*/
    unexp_error = 1;
    return;
}

/* Test execution *******************************************************   */
int test_main(void) {
    /* Test generic variables  ******************************************	*/
    /* function to test return code     */
    rtems_status_code ret = RTEMS_SUCCESSFUL;
    air_partition_status_t status;
    /* total test result                */
    int res = TEST_SUCCESS;
    /* wait ticks before test end to print results                    */
    int wait = 200; // MTF is 0.4

    /* Test specific variables  ******************************************	*/
    int counter = 0;
    rtems_interval t1 = 0;
    rtems_interval t2 = 0;


    /* Test step 0 code */
    for (;;) {
        /* EXPECTED: 
           The rtems_clock_get function call should be successfull */
        ret |= _rtems_clock_get(&t1);
        /* EXPECTED: 
           The rtems_task_wake_after function call should be successfull */

        ret |= rtems_task_wake_after(2);
        
        /* EXPECTED: 
           The rtems_clock_get function call should be successfull */
        ret |= _rtems_clock_get(&t2);
        air_syscall_get_partition_status(-1, &status);
        printf ("Partition %d: Window %d\n", status.identifier, status.window_id);
    } /* end for */

    return 0;
}
