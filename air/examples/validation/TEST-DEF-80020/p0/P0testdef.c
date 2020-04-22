
/* ***********************************************************************    */
/* ** TEST-DEF-80020 / 80020 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0010                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	The pmk_linkcmds_vars.h holds references to the text, data and bss
	sections of each partition. This test shall try to branch into each of these
	sections from all partitions (other than the test partition itself). Each attempt
	shall raise a Health Monitor event that shall be captured. */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P0testdef.h>


/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/
typedef void (* function_pointer_t) (void );

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 30;

/* Test auxiliary functions    ********************************************	*/
rtems_task Thread1(rtems_task_argument argument) {
	
	/* function to test return code     */
    rtems_status_code ret    = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res     = TEST_SUCCESS;     

	/* Test specific variables  ******************************************	*/
	/* function pointer */
	function_pointer_t function = NULL;

    while(1) {
    	/* Test Step 4 
    		In task 0: Get the address of the start of the data section of
			partition 1. */
		test_step_announce(4,1);

		/* Test step 3 code */
		function = (function_pointer_t) &air_memmap_part_data1_start;
		ret |= RTEMS_SUCCESSFUL;

		/* EXPECTED: */
		if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
			res &= test_step_report(    TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_step_report(    TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										unexp_error);
    		test_exit(res,mtf_ticks);
		}

		/* Test Step 5 
			In task 0: Define a function pointer and make it point to the
		obtained address. Call the function. */
		test_step_announce(5,1);

		/* Test step 5 code */
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
			test_exit(res,mtf_ticks);
		}

		function();

		/* this should never execute */
		unexp_error = -1;
		rtems_task_wake_after(mtf_ticks * 4);
    }
}

rtems_task Thread2(rtems_task_argument argument) {
	
	/* function to test return code     */
    rtems_status_code ret    = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res     = TEST_SUCCESS;     

	/* Test specific variables  ******************************************	*/
	/* function pointer */
	function_pointer_t function = NULL;

    while(1) {
	   /* Test Step 7 
			In task 1: Get the address of the start of the bss section of
		partition 1. */
		test_step_announce(7,1);

		/* Test step 7 code */
		function = (function_pointer_t) &air_memmap_part_bss1_start;
		ret |= RTEMS_SUCCESSFUL;

		/* EXPECTED: */
		if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
			res &= test_step_report(    TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_step_report(    TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										unexp_error);
    		test_exit(res,mtf_ticks);
		}

		/* Test Step 8 
			In task 1: Define a function pointer and make it point to the
		obtained address. Call the function. */
		test_step_announce(8,1);

		/* Test step 8 code */
		ret |= RTEMS_SUCCESSFUL;

		/* EXPECTED: */
		if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
			res &= test_step_report(    TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_step_report(    TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										unexp_error);
			test_exit(res,mtf_ticks);
		}

		function();

		/* this should never execute */
		unexp_error = -1;
		rtems_task_wake_after(mtf_ticks * 4);
    }
}

rtems_task Thread3(rtems_task_argument argument) {
	
	/* function to test return code     */
    rtems_status_code ret    = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res     = TEST_SUCCESS;     

	/* Test specific variables  ******************************************	*/
	/* function pointer */
	function_pointer_t function = NULL;

    while(1) {
    	/* Test Step 10 
    		In task 2: Get the address of the start of the text section of
			partition 1. */
		test_step_announce(10,1);

		/* Test step 10 code */
		function = (function_pointer_t) &air_memmap_part_text1_start;
		ret |= RTEMS_SUCCESSFUL;

		/* EXPECTED: */
		if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
			res &= test_step_report(    TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_step_report(    TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										unexp_error);
    		test_exit(res,mtf_ticks);
		}

		/* Test Step 11 
			In task 2: Define a function pointer and make it point to the
		obtained address. Call the function. */
		test_step_announce(11,1);

		/* Test step 5 code */
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
			test_exit(res,mtf_ticks);
		}

		function();

		/* this should never execute */
		unexp_error = -1;
		rtems_task_wake_after(mtf_ticks * 4);
    }
}



/* Test HM callbacks        ***********************************************	*/
void hm_part_callback (pmk_hm_err_id_e i_error,void *i_state) {
	/* signal error ocurrence	*/
    unexp_error	= i_error;
    return;
}

/* Test execution *******************************************************    */
int test_main (void) {
    /* Test generic variables  ******************************************	*/
	/* function to test return code     */
    rtems_status_code ret    = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res     = TEST_SUCCESS;     
    
    /* Test specific variables  ******************************************	*/
	/* dummy variable where we'll try to jump to */

	/* Thread IDs */
	rtems_id thread1ID;
	rtems_id thread2ID;
	rtems_id thread3ID;
	
    /* Test Start ******************************************************    */
    test_enter(80020);

	/* Wait for test step 0 */
	rtems_task_wake_after(mtf_ticks>>1);
	
    /* Test Steps *******************************************************    */
    /* Test Step 1 
    	Create four low priority tasks (with increasing priorities). */
    test_step_announce(1,1);

    /* Test step 1 code */
	ret |= rtems_task_create(rtems_build_name(	'T' , 'H' , 'R' , '1') ,
		                               	31 ,
					                   	RTEMS_MINIMUM_STACK_SIZE ,
									   	RTEMS_PREEMPT ,
										RTEMS_NO_FLOATING_POINT ,
										&thread1ID);
	ret |=  rtems_task_create(rtems_build_name(	'T' , 'H' , 'R' , '2') ,
		                              	32 ,
					                   	RTEMS_MINIMUM_STACK_SIZE ,
									   	RTEMS_PREEMPT ,
										RTEMS_NO_FLOATING_POINT ,
										&thread2ID);
   	ret |=  rtems_task_create(rtems_build_name(	'T' , 'H' , 'R' , '3') ,
		                              	33 ,
					                   	RTEMS_MINIMUM_STACK_SIZE ,
									   	RTEMS_PREEMPT ,
										RTEMS_NO_FLOATING_POINT ,
										&thread3ID);
 
    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    	test_exit(res,mtf_ticks);
    }

    /* Test Step 2 
    	Start the low priority tasks. */
    test_step_announce(2,1);

    /* Test step 2 code */
	ret |= rtems_task_start(thread1ID , Thread1 , 0);
	ret |= rtems_task_start(thread2ID , Thread2 , 0);
	ret |= rtems_task_start(thread3ID , Thread3 , 0);

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    	test_exit(res,mtf_ticks);
    }

    /* Test Step 3 
    	Suspend main task (rtems_task_wake_after(MFT)). */
    test_step_announce(3,1);

    /* Test step 3 code */
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
    	test_exit(res,mtf_ticks);
    }

    ret |= rtems_task_wake_after(1); 

    /* Test Step 6 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(6,1);

    /* Test step 6 code */
    ret |= rtems_task_suspend(thread1ID);

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (PMK_SEG_ERR == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
		unexp_error = 0;
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    unexp_error);
    	test_exit(res,mtf_ticks);
    }
	rtems_task_wake_after(1);

    /* Test Step 9 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(9,1);

    /* Test step 9 code */
    ret |= rtems_task_suspend(thread2ID);

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (PMK_SEG_ERR == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
		unexp_error = 0;
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    	test_exit(res,mtf_ticks);
    }
	rtems_task_wake_after(1);

    /* Test Step 12 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(12,1);

    /* Test step 12 code */
	ret |= rtems_task_suspend(thread3ID);

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (PMK_SEG_ERR == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
		unexp_error = 0;
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    	test_exit(res,mtf_ticks);
    }
	rtems_task_wake_after(mtf_ticks*2);

    
    /* Test End */
    test_return();

    return 0;
}


