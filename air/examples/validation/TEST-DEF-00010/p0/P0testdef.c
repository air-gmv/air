
/* ***********************************************************************    */
/* ** TEST-DEF-00010 / 10 *********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0120                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Call the SET_MODULE_SCHEDULE function from an authorized partition and
        check that the partitioning schedule is altered starting at the next major time
        frame (MTF) start (with GET_PARTITION_MODE). Check that a call to
        SET_MODULE_SCHEDULE with the current schedule produces no change. Check that the call from a
        non-authorized partition fails with an HM process/ partition event. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P0testdef.h>

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
    /* total test result                */
    int res = TEST_SUCCESS;
    /* wait ticks before test end to print results                    */
    int wait = 150; // MTF is 0.3

    printf ("Starting TEST-DEF-00010\n");

    /* Test specific variables  ******************************************	*/
    int counter = 0;
    rtems_interval t1 = 0;
    rtems_interval t2 = 0;

    /* Test Start ******************************************************    */
    test_enter(10);

    /* since we are measuring execution windows, we should not use the
     * current one, since it was already partially occupied by the 
     * partition rtems boot-up, so let's wait until the next one	*/
    rtems_task_wake_after(wait >> 1);

    /* Test Steps *******************************************************    */
    /* Test Step 0 
           Get the current time, sleep for one tick, get the time again and 
           measure the difference. Must be equal to 1 for at least 19 times. */
    test_step_announce(0, 1);
    /* Test step 0 code */
    for (counter = 0; counter < 19; counter++) {
        /* EXPECTED: 
           The rtems_clock_get function call should be successfull */
        ret |= _rtems_clock_get(&t1);
        /* EXPECTED: 
           The rtems_task_wake_after function call should be successfull */
        ret |= rtems_task_wake_after(1);
        /* EXPECTED: 
           The rtems_clock_get function call should be successfull */
        ret |= _rtems_clock_get(&t2);
        /* EXPECTED: 
           The clock tick difference should be equal to 1 */

        ret |= t2 == t1 + 1 ? RTEMS_SUCCESSFUL : 1;


    } /* end for */
    if (RTEMS_SUCCESSFUL == ret) {
        res &= test_report(__FILE__, __LINE__, TEST_SUCCESS,
                RESULT_EQUAL | RESULT_TYPE_VALUE,
                ret);
    } else {
        res &= test_report(__FILE__, __LINE__, TEST_FAILURE,
                RESULT_DIFF | RESULT_TYPE_VALUE,
                t2 - t1);
    }

    rtems_task_wake_after(1);

    /* Test Step 2
        Check that the last step differs (MTF - P0) ticks execution window from 
     *  test_step_announce is blocked waiting for steip 1,1
     
     * New passes  (50-19) from p0, 50ofP1 + 50ofP2 so 131 in TOTAL
                the previously stored value.  */
    test_step_announce(2, 1);

    /* Test step 2 code */
    ret = RTEMS_SUCCESSFUL;

    /* EXPECTED: 
       The rtems_clock_get function call should be successfull */
    ret |= _rtems_clock_get(&t1);


    ret |= t1 == t2 + 131 ? RTEMS_SUCCESSFUL : 1;


    /* EXPECTED: */
    if (RTEMS_SUCCESSFUL == ret) {
        res &= test_report(__FILE__, __LINE__, TEST_SUCCESS,
                RESULT_EQUAL | RESULT_TYPE_VALUE,
                ret);
    } else {
        res &= test_report(__FILE__, __LINE__, TEST_FAILURE,
                RESULT_DIFF | RESULT_TYPE_VALUE
                , t1 - t2);
    }

    rtems_task_wake_after(wait);

    /* Test Step 3
        Call SET_MODULE_CHANGE for schedule "alternative_sched". */
    test_step_announce(3, 1);

    /* Test step 3 code */
    SCHEDULE_ID_TYPE sched_id;
    RETURN_CODE_TYPE rc;

    GET_MODULE_SCHEDULE_ID("test_sched2", &sched_id, &rc);
    ret |= rc;

    SET_MODULE_SCHEDULE(sched_id, &rc);
    ret |= rc;

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error)) {
        res &= test_report(__FILE__, __LINE__, TEST_SUCCESS,
                RESULT_EQUAL | RESULT_TYPE_VALUE,
                ret);
    } else {
        res &= test_report(__FILE__, __LINE__, TEST_FAILURE,
                RESULT_DIFF | RESULT_TYPE_VALUE
                , ret);
    }

/* Test Step 4
        Get the current time, sleep for one tick, get the time again and 
           measure the difference. Must be equal to 1 for at least 19 times (the 
           same value as before), since the schedule change only takes effect in 
           the next MTF */
    test_step_announce(4, 1);

    /* Test step 4 code */
    for (counter = 0; counter < 19; counter++) {
        /* EXPECTED: 
           The rtems_clock_get function call should be successfull */
        ret |= _rtems_clock_get(&t1);

        /* EXPECTED: 
           The rtems_clock_get function call should be successfull */
        ret |= rtems_task_wake_after(1);

        /* EXPECTED: 
           The rtems_clock_get function call should be successfull */
        ret |= _rtems_clock_get(&t2);

        /* EXPECTED: 
           The clock tick difference is equal to 1 */
        ret |= t2 == t1 + 1 ? RTEMS_SUCCESSFUL : 1;

    } /* end for */

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error)) {
        res &= test_report(__FILE__, __LINE__, TEST_SUCCESS,
                RESULT_EQUAL | RESULT_TYPE_VALUE,
                ret);
    } else {
        res &= test_report(__FILE__, __LINE__, TEST_FAILURE,
                RESULT_DIFF | RESULT_TYPE_VALUE
                , t1 - t2);
    }
    // pushing to end of window 31 ticks, schedule will change!
    rtems_task_wake_after(31);

    /* Test Step 5 
        Get the current time, and check that it differs MTF from previously
        stored value. */
    test_step_announce(5, 1);

    /* Test step 5 code */

    /* EXPECTED: 
       The rtems_clock_get function call should be successfull */
    ret |= _rtems_clock_get(&t1);

    /* EXPECTED: 
       The clock tick difference should be equal to 131, end of previous schedule */
    ret |= t1 == t2 + 131 ? RTEMS_SUCCESSFUL : 1;


    /* EXPECTED: */
    if (RTEMS_SUCCESSFUL == ret) {
        res &= test_report(__FILE__, __LINE__, TEST_SUCCESS,
                RESULT_EQUAL | RESULT_TYPE_VALUE,
                ret);
    } else {
        res &= test_report(__FILE__, __LINE__, TEST_FAILURE,
                RESULT_DIFF | RESULT_TYPE_VALUE
                , t1 - t2);
    }
    
    //Jump one MTF for initializations
    rtems_task_wake_after(750);


    /* Test Step 6 
        Get the current time, sleep for one tick, get the time again and 
           measure the difference. Must be equal to 1 for at least 9 times (the 
           value from the new schedule). */
    test_step_announce(6, 1);

    /* Test step 6 code */
    for (counter = 0; counter < 9; counter++) {
        /* EXPECTED: 
           The rtems_clock_get function call should be successfull */
        ret |= _rtems_clock_get(&t1);

        /* EXPECTED: 
           The rtems_clock_get function call should be successfull */
        ret |= rtems_task_wake_after(1);

        /* EXPECTED: 
           The rtems_clock_get function call should be successfull */
        ret |= _rtems_clock_get(&t2);

        /* EXPECTED: 
           The clock tick difference is equal to 1 */
        ret |= t2 == t1 + 1 ? RTEMS_SUCCESSFUL : 1;
    } /* end for */

    if (RTEMS_SUCCESSFUL == ret) {
        res &= test_report(__FILE__, __LINE__, TEST_SUCCESS,
                RESULT_EQUAL | RESULT_TYPE_VALUE,
                ret);
    } else {
        res &= test_report(__FILE__, __LINE__, TEST_FAILURE,
                RESULT_DIFF | RESULT_TYPE_VALUE,
                t2 - t1);
    }
    // schedule push to end of windows.
    rtems_task_wake_after(240);


    /* Test Step 7 
        Get the current time, and check that it differs MTF from previously
        stored value. */
    test_step_announce(7, 1);

    /* Test step 7 code */
    /*   The rtems_clock_get function call should be successfull */
    ret |= _rtems_clock_get(&t1);

    /* EXPECTED: 
       The clock tick difference should be equal to 741 */
    //p0 250-9 + p2 250 + p3 250
    ret |= t1 == t2 + 741 ? RTEMS_SUCCESSFUL : 1;

    /* EXPECTED: */
    if (RTEMS_SUCCESSFUL == ret) {
        res &= test_report(__FILE__, __LINE__, TEST_SUCCESS,
                RESULT_EQUAL | RESULT_TYPE_VALUE,
                ret);
    } else {
        res &= test_report(__FILE__, __LINE__, TEST_FAILURE,
                RESULT_DIFF | RESULT_TYPE_VALUE
                , t1 - t2);
    }
    /* Test End */
    test_finish(res);

    return 0;
}

