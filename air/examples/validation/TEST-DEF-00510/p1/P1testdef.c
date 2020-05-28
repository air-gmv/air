
/* ***********************************************************************    */
/* ** TEST-DEF-00510 / 510 ********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0430                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Test if a branch into an unaligned address (into an otherwise 'valid' code
        address) is detected by the LEON, and the associated fault detected by the HM.
        Check that the remaining partitions are not affected. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>


/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/
typedef unsigned int (*fptr)(int a, int b);

/* Test global variables **************************************************	*/
int unexp_error = 0;
int mtf_ticks = 30;

/* Test auxiliary functions    ********************************************	*/

unsigned int myfunc(int a, int b) {
    int c;

    c = a << b;
    c = a * c * b >> a;

    return c + a + b;

}

rtems_task error_gen(rtems_task_argument unused) {
    volatile unsigned int aux;
    fptr aux_func;

    /* function to test return code     */
    rtems_status_code ret = RTEMS_SUCCESSFUL;
    /* total test result                */
    int res = TEST_SUCCESS;

    /* Test Step 4 
    In task, Branch into an unaligned address. Verify that a trap was
    raised. */
    test_step_announce(4, 1);

    /* obtain pointer to func */
    aux = (int) myfunc;

    /* unalign the pointer */
    aux++;

    /* cast pointer to func */
    aux_func = (fptr) aux;

    /* call unaligned function */
    (aux_func) (5, 4);


    rtems_task_wake_after(10);
}

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id, air_error_e i_error) {
    /* signal error ocurrence	*/
    printf("HM CALLBACK error %d\n", i_error);
    unexp_error = i_error;
    int res2 = 0;
    /* Expecting Segmentation error*/
    if (unexp_error == AIR_SEGMENTATION_ERROR) {
        res2 = test_report(__FILE__, __LINE__, TEST_SUCCESS,
                RESULT_EQUAL | RESULT_TYPE_VALUE,
                1);
    } else {
        res2 = test_report(__FILE__, __LINE__, TEST_FAILURE,
                RESULT_DIFF | RESULT_TYPE_VALUE,
                unexp_error);
        test_exit(TEST_FAILURE, mtf_ticks >> 1);
    }

    test_return(res2);
    return;
}

/* Test execution *******************************************************    */
int test_main(void) {
    /* Test generic variables  ******************************************	*/
    /* repeatition iteration counter	*/
    int repeat = 0;
    /* function to test return code     */
    rtems_status_code ret = RTEMS_SUCCESSFUL;
    /* total test result                */
    int res = TEST_SUCCESS;

    /* Test specific variables  ******************************************	*/

    /* tasks names */
    rtems_name tname;

    /* array that will contain the tasks ids */
    rtems_id tid;

    /* Test Start ******************************************************    */
    test_enter(510);
    debug_libtest();
    
    /* Test Steps *******************************************************    */
    /* Test Step 1 
        Create a Task with low priority that will branch to a unaligned
        address */
    test_step_announce(1, 1);

    /* Test step 1 code */
    tname = rtems_build_name('E', 'R', 'R', 'G');

    /* Create Task */
    ret |= rtems_task_create(tname, 100, 4096,
            RTEMS_DEFAULT_MODES,
            RTEMS_DEFAULT_ATTRIBUTES, &tid);

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error)) {
        res &= test_report(__FILE__, __LINE__, TEST_SUCCESS,
                RESULT_EQUAL | RESULT_TYPE_VALUE,
                ret);
    } else {
        res &= test_report(__FILE__, __LINE__, TEST_FAILURE,
                RESULT_DIFF | RESULT_TYPE_VALUE,
                ret);
        test_exit(TEST_FAILURE, mtf_ticks >> 1);
    }

    /* Test Step 2 
        Start the previous task. */
    test_step_announce(2, 1);

    /* Test step 2 code */
    ret |= rtems_task_start(tid, error_gen, (rtems_task_argument) NULL);

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error)) {
        res &= test_report(__FILE__, __LINE__, TEST_SUCCESS,
                RESULT_EQUAL | RESULT_TYPE_VALUE,
                ret);
    } else {
        res &= test_report(__FILE__, __LINE__, TEST_FAILURE,
                RESULT_DIFF | RESULT_TYPE_VALUE,
                ret);
        test_exit(TEST_FAILURE, mtf_ticks >> 1);
    }

    /* Test Step 3 
        Place current task in sleep mode for 1 tick */
    test_step_announce(3, 1);


    /* EXPECTED: */
    if (0 == unexp_error) {
        res &= test_report(__FILE__, __LINE__, TEST_SUCCESS,
                RESULT_EQUAL | RESULT_TYPE_VALUE,
                ret);
    } else {
        res &= test_report(__FILE__, __LINE__, TEST_FAILURE,
                RESULT_DIFF | RESULT_TYPE_VALUE,
                ret);
        test_exit(TEST_FAILURE, mtf_ticks >> 1);
    }

    /* Test step 3 code */
    rtems_task_wake_after(2);

    /* Test End */
    test_return(res);
    return 0;
}


