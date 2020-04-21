
/* ***********************************************************************    */
/* ** TEST-DEF-90001 / 90001 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0120                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Configure a test with 2 partitions, in which one of them has 2 tasks. One
	of the tasks call a function that sets the frame pointer to 0. Checks if that
	function blocks or not the other partition and/or the other task. */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P0testdef.h>


/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 150;

rtems_id task_id1, task_id2;

/* Test auxiliary functions    ********************************************	*/
void task1();
void task2();
inline void pmk_null_stack_pointer(void);
/* Test HM callbacks        ***********************************************	*/
void hm_part_callback (pmk_hm_err_id_e i_error,void *i_state) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    pmk_hm_state_id_e state = (pmk_hm_state_id_e) i_state;
	/* signal error ocurrence	*/
    unexp_error	= 1;
    return;
}

/* Test execution *******************************************************    */
int test_main (void) {
    /* Test generic variables  ******************************************	*/
	/* repeatition iteration counter	*/ 
	int repeat     = 0;                
	/* function to test return code     */
    rtems_status_code ret    = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res     = TEST_SUCCESS;     
    
    /* Test specific variables  ******************************************	*/
	rtems_name	name1 = 001,
				name2 = 010;
    /* Test Start ******************************************************    */
    test_enter(90001);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Create a task, with priority 1 and stack_size of 4096. */
    test_step_announce(0,1);

    /* Test step 0 code */
    ret = rtems_task_create (	name1, 
								1, 
								4096, 
								RTEMS_PREEMPT, 
								RTEMS_PREEMPT_MASK,  
								&task_id1 );

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 1 
    	Create a task, with priority 100 and stack_size of 4096. */
    test_step_announce(1,1);

    /* Test step 1 code */
    ret = rtems_task_create (	name2, 
								100, 
								4096, 
								RTEMS_PREEMPT, 
								RTEMS_PREEMPT_MASK,  
								&task_id2 );

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 2 
    	Start task #1. */
    test_step_announce(2,1);

    /* Test step 2 code */
    ret = rtems_task_start ( task_id1, task1, 0 );

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 3 
    	Start task #2. */
    test_step_announce(3,1);

    /* Test step 3 code */
    ret = rtems_task_start ( task_id2, task2, 1 );

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
    

    return 0;
}

/*
 * 	TASK 1
 */
void task1(){
	/* function to test return code     */
    rtems_status_code ret = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res = TEST_SUCCESS;

    /* Test Step 4 
    	Call the function pmk_null_stack_pointer. */
    test_step_announce(4,1);

    /* Test step 4 code */

	ret |= RTEMS_SUCCESSFUL;

	/* EXPECTED: */
	if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
		res &= test_step_report(    TEST_SUCCESS,
									RESULT_EQUAL | RESULT_TYPE_VALUE,
									ret);
	} else {    
		res &= test_step_report(    TEST_FAILURE,
									RESULT_DIFF | RESULT_TYPE_VALUE,
									ret);
	}

	pmk_null_stack_pointer();
	
	rtems_task_wake_after(50);
	
    /* Test End */
    test_exit(res,mtf_ticks);
	return;
}

inline void pmk_null_stack_pointer(void) {
	/* function to test return code     */
    rtems_status_code ret = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res = TEST_SUCCESS;     

    /* Test Step 5 
    	Set the frame pointer to 0. */
    test_step_announce(5,1);

    /* Test step 5 code */
	asm volatile( "mov 0, %fp" );
    ret |= RTEMS_SUCCESSFUL;

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
}

/*
 * 	TASK 2
 */
void task2(){
	/* function to test return code     */
    rtems_status_code ret = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res = TEST_SUCCESS;
	
	/* Test Step 6 
		Test if the task is blocked or not. */
	test_step_announce(6,1);

	/* Test step 6 code */

	ret |= RTEMS_SUCCESSFUL;

	/* EXPECTED: */
	if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
		res &= test_step_report(    TEST_SUCCESS,
									RESULT_EQUAL | RESULT_TYPE_VALUE,
									ret);
	} else {    
		res &= test_step_report(    TEST_FAILURE,
									RESULT_DIFF | RESULT_TYPE_VALUE,
									ret);
	}

	rtems_task_wake_after(50);
	/* sleeps again, so the step is executed just 1 time */
	rtems_task_wake_after(50);
}