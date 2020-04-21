
/* ***********************************************************************    */
/* ** TEST-DEF-01371 / 1371 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0140                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define two schedules: 0) P0, P1, P2 1) P1, P3, P2. Modify TEST-DEF-80380
	in order to determine that the Module Schedule operation has no impact in the
	floating point permissions of the relevant partitions. Verify the Floating point
	context remains across module changes. */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P2testdef.h>

#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 100;

/* Test auxiliary functions    ********************************************	*/

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
	volatile float a;
	volatile float pi;
	RETURN_CODE_TYPE RC;
	int test_err = 0;
	SCHEDULE_ID_TYPE sched = 0;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 3 
    	Perform a cumulative floating point operation and check that no
	health monitor event was generated. */
    test_step_announce(3,1);

    /* Test step 3 code */
    a = 2.25+3.75;

    /* EXPECTED: */
    if ((6 == a) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 4 
    	Change to the second module schedule. Wait for next MTF. */
    test_step_announce(4,1);

    /* Test step 4 code */
    GET_MODULE_SCHEDULE_ID("alternative", &sched, &RC);
	if (NO_ERROR != RC){
		test_err = 1;
	}

	RC = -1;
	SET_MODULE_SCHEDULE(sched, &RC);

    /* EXPECTED: */
    if ((NO_ERROR == RC) && (test_err == 0) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	
	rtems_task_wake_after(mtf_ticks/2);
	
    /* Test Step 7 
    	Perform a cumulative floating point operation and check that no
	health monitor event was generated. */
    test_step_announce(7,1);

    /* Test step 7 code */
    pi = a*2;
	
    /* EXPECTED: */
    if ((12 == pi) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 8 
    	Change to the original module schedule. Wait for next MTF. */
    test_step_announce(8,1);

    /* Test step 8 code */
    GET_MODULE_SCHEDULE_ID("test_sched", &sched, &RC);
	if (NO_ERROR != RC){
		test_err = 1;
	}

	RC = -1;
	SET_MODULE_SCHEDULE(sched, &RC);

    /* EXPECTED: */
    if ((NO_ERROR == RC) && (test_err == 0) &&  (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	
	rtems_task_wake_after(mtf_ticks/2);
	
    /* Test Step 11 
    	Perform a cumulative floating point operation and check that no
	health monitor event was generated. */
    test_step_announce(11,1);

    /* Test step 11 code */
    pi = pi + a + 1.25;

    /* EXPECTED: */
    if ((19.25 == pi) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    
    /* Test End */
    test_return();
    return 0;
}


