
/* ***********************************************************************    */
/* ** TEST-DEF-90000 / 90000 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0120                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Tests if a for(;;); executed by a timer hangs the system or not. */

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

rtems_id task_id;
/* Test auxiliary functions    ********************************************	*/
void timer();
void for_r( rtems_id timer_id, rtems_id id);

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
	rtems_attribute		attribute_set;
	rtems_name	name1 = 001;
	
    /* Test Start ******************************************************    */
    test_enter(90000);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Create task, with priority 10 and stack_size of 4096. */
    test_step_announce(0,1);

    /* Test step 0 code */
	ret = rtems_task_create (	name1, 
								10, 
								4096, 
								RTEMS_PREEMPT, 
								RTEMS_PREEMPT_MASK,  
								&task_id );

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
    	Start task. */
    test_step_announce(1,1);

    /* Test step 1 code */
	ret = rtems_task_start ( task_id, timer, 0 );

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

/* ****************************************************************************
 *								"TIMER" TASK
 * ************************************************************************* */
void timer() {
	rtems_status_code	status 		= RTEMS_UNSATISFIED;
	rtems_interval 		fire_after	= 0;
	rtems_name 			timer_name  = 001;
	rtems_id			timer_id;
	
	rtems_status_code ret    = RTEMS_SUCCESSFUL; 
	int res     = TEST_SUCCESS;
	
    /* Test Step 2 
    	Creates timer. */
    test_step_announce(2,1);

    /* Test step 2 code */
	ret = rtems_timer_create( timer_name, &timer_id);

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
		Sets timer to fire after 10 Clock Ticks. */
	test_step_announce(3,1);

	/* Test step 3 code */

	fire_after = 20;
	
	ret = rtems_timer_fire_after( timer_id, fire_after, for_r, task_id );

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

	/* Test Step 4 
		Sets task to sleep in 20 clock ticks. */
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

	rtems_task_wake_after(50);
	
    test_return();
}

/* ****************************************************************************
 *								"FOR" ROUTINE
 * ************************************************************************* */
void for_r( rtems_id timer_id, rtems_id id) {

	rtems_status_code ret    = RTEMS_SUCCESSFUL; 
	int res     = TEST_SUCCESS; 
	
    /* Test Step 5 
    	Tests if the other partitions block with a for loop. */
    test_step_announce(5,1);
	
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
	
	/* Test step 5 code */
	for(;;);
	
}

