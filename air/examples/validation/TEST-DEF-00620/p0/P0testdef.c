
/* ***********************************************************************    */
/* ** TEST-DEF-00620 / 620 ********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0430                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Test if the execution of a jump instruction into the data area, for both
	valid and invalid opcodes, in a user mode (non-previledged) partition is
	captured by the LEON and handled by the HM, and the associated HM action is called.
	Check that the remaining partitions are not affected. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

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
	/* dummy variable where we'll try to jump to */
	int var[16];
	/* function pointer */
	function_pointer_t function = NULL;

	int i = 0;

    while(1) {
		/* Test Step 3 
		Define a function pointer and make it point to the address of an
		internal variable. Initialise that variable to all zeros (invalid opcode).
		Call the function. */
		test_step_announce(3,1);

		/* Test step 3 code */
		function = (function_pointer_t) &var;
		for (i=0;i<16;i++) var[i] = 0;
		ret |= RTEMS_SUCCESSFUL;

		/* EXPECTED: */
		if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
			res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
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
	/* dummy variable where we'll try to jump to */
	int var[16];
	/* function pointer */
	function_pointer_t function = NULL;

	int i = 0;

    while(1) {
		/* Test Step 6 
    		Modify the contents of the variable to 0x01000000 (nop).
			Call the function again. */    
		test_step_announce(6,1);

		/* Test step 6 code */
		function = (function_pointer_t) &var;
		for (i=0;i<16;i++) var[i] = 0x01000000;
		ret |= RTEMS_SUCCESSFUL;

		/* EXPECTED: */
		if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
			res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}

		function();


		/* this should never execute */
		unexp_error = -1;
		rtems_task_wake_after(mtf_ticks * 4);
 
    }
}

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    
	/* signal error ocurrence	*/
    unexp_error	= i_error;
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
	/* dummy variable where we'll try to jump to */
	int var[16];
	/* function pointer */
	function_pointer_t function = NULL;

	int i = 0;

	/* Thread IDs */
	rtems_id thread1ID;
	rtems_id thread2ID;
	
    /* Test Start ******************************************************    */
    test_enter(620);
    
    /* Test Steps *******************************************************    */

	rtems_task_wake_after(mtf_ticks >> 1);

	/* Test Step 1
	 * 	Partition 0:
	 	Create the two tasks where the faults will occur.
		Each one will be innaccessible after the respective fault. 	*/
	test_step_announce(1,1);

	ret |= rtems_task_create(rtems_build_name(	'T' , 'H' , 'R' , '1') ,
		                               	30 ,
					                   	RTEMS_MINIMUM_STACK_SIZE ,
									   	RTEMS_PREEMPT ,
										RTEMS_NO_FLOATING_POINT ,
										&thread1ID);
	ret |=  rtems_task_create(rtems_build_name(	'T' , 'H' , 'R' , '2') ,
		                              	29 ,
					                   	RTEMS_MINIMUM_STACK_SIZE ,
									   	RTEMS_PREEMPT ,
										RTEMS_NO_FLOATING_POINT ,
										&thread2ID);
    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

	/* Test Step 2
	 	Start the first task faulty and suspend self.	*/
	test_step_announce(2,1);

	ret |= rtems_task_start(thread1ID , Thread1 , 0);
	
	/* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

	rtems_task_wake_after(1);


    /* Test Step 4 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */    
    test_step_announce(4,1);

    /* Test step 4 code */
	if (PMK_SEG_ERR == unexp_error) {
	    ret = RTEMS_SUCCESSFUL;
		unexp_error = 0;
	}

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    unexp_error);
    }

	/* Test Step 5
	 	Start the second faulty task and suspend self.	*/
	test_step_announce(5,1);

	ret |= rtems_task_start(thread2ID , Thread2 , 0);
	
	/* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	rtems_task_wake_after(1);


    /* Test Step 7 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */    
	test_step_announce(7,1);

    /* Test step 7 code */
   	if (PMK_SEG_ERR == unexp_error) {
	    ret = RTEMS_SUCCESSFUL;
		unexp_error = 0;
	}

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    unexp_error);
    }

 	rtems_task_wake_after(mtf_ticks );

    /* Test End */
    test_exit(res, mtf_ticks);
    return 0;
}

