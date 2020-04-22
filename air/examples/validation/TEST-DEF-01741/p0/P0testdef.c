
/* ***********************************************************************    */
/* ** TEST-DEF-01741 / 1741 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0700                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Create a test for the GET_SAMPLING_PORT_CURRENT_STATUS function. */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P0testdef.h>

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
	SAMPLING_PORT_ID_TYPE PORT0;
    SAMPLING_PORT_ID_TYPE PORT1;
	SAMPLING_PORT_NAME_TYPE NAME0 = "A1_S";
	SAMPLING_PORT_NAME_TYPE NAME1 = "B1_S";
	MESSAGE_SIZE_TYPE SIZE = 4;
	MESSAGE_SIZE_TYPE RSIZE = 0;
	VALIDITY_TYPE VALIDITY = 0;
	SYSTEM_TIME_TYPE PERIOD = 2000000000;
	RETURN_CODE_TYPE RC;
	int error = 0;
	unsigned char msg[8];
	SAMPLING_PORT_CURRENT_STATUS_TYPE STATUS;
	
    /* Test Start ******************************************************    */
    test_enter(1741);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Call GET_SAMPLING_PORT_CURRENT_STATUS with an invalid port id
	(Invalid Param). */
    test_step_announce(0,1);

    /* Test step 0 code */
    GET_SAMPLING_PORT_CURRENT_STATUS (PORT0, &STATUS, &RC);

    /* EXPECTED: */
    if ((INVALID_PARAM == RC) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 1 
    	Create two sampling ports with distinct directions (no error). */
    test_step_announce(1,1);

    /* Test step 1 code */
    CREATE_SAMPLING_PORT (NAME0, SIZE, SOURCE, PERIOD, &PORT0, &RC);
	if(RC != NO_ERROR){
		error = 1;
	}

	CREATE_SAMPLING_PORT (NAME1, SIZE, DESTINATION, PERIOD, &PORT1, &RC);
	if(RC != NO_ERROR){
		error = 1;
	}
	
    /* EXPECTED: */
    if ((0 == error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 2 
    	Get current status from the destination port. Verify the port is
	empty. */
    test_step_announce(2,1);

    /* Test step 2 code */
    GET_SAMPLING_PORT_CURRENT_STATUS (PORT1, &STATUS, &RC);

    /* EXPECTED: */
    if ((NO_ERROR == RC) && (PERIOD == STATUS.REFRESH_PERIOD) && 
	    (SIZE == STATUS.MAX_MESSAGE_SIZE) &&
		(DESTINATION == STATUS.PORT_DIRECTION) &&
	    (STALE == STATUS.MESSAGE_AGE) && (EMPTY_PORT == STATUS.UPDATED) &&
		(0 == STATUS.TIME_STAMP) && (0 == unexp_error))  {
		
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 3 
    	Get current status from the source port. Verify the port is empty. */
    test_step_announce(3,1);

    /* Test step 3 code */
    GET_SAMPLING_PORT_CURRENT_STATUS (PORT0, &STATUS, &RC);

    /* EXPECTED: */
    if ((NO_ERROR == RC) && (PERIOD == STATUS.REFRESH_PERIOD) && 
	    (SIZE == STATUS.MAX_MESSAGE_SIZE) &&
		(SOURCE == STATUS.PORT_DIRECTION) &&
	    (STALE == STATUS.MESSAGE_AGE) && (EMPTY_PORT == STATUS.UPDATED) &&
		(0 == STATUS.TIME_STAMP) && (0 == unexp_error))  {
		
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 4 
    	Wait for next mtf */
    test_step_announce(4,1);

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
	
	/* wait for next mtf */
	rtems_task_wake_after(mtf_ticks);
	
    /* Test Step 8 
    	Get current status from the destination port. Verify the port has a
	new, fresh, message. Read the message. */
    test_step_announce(8,1);

    /* Test step 8 code */
	error = 0;
    GET_SAMPLING_PORT_CURRENT_STATUS(PORT1, &STATUS, &RC);
	if(RC != NO_ERROR){
		error = 1;
	}
	
	READ_SAMPLING_MESSAGE (PORT1, &msg[0], &RSIZE, &VALIDITY, &RC);
	
    /* EXPECTED: */
    if ((NO_ERROR == RC) && (0 == error) && (PERIOD == STATUS.REFRESH_PERIOD) && 
	    (SIZE == STATUS.MAX_MESSAGE_SIZE) &&
		(DESTINATION == STATUS.PORT_DIRECTION) &&
	    (FRESH == STATUS.MESSAGE_AGE) && (NEW_MESSAGE == STATUS.UPDATED) &&
		(0 < STATUS.TIME_STAMP) && (0 == unexp_error))  {
		
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	
    /* Test Step 9 
    	Get current status from the destination port. Verify the message is
	reported as consumed. */
    test_step_announce(9,1);
    /* Test step 9 code */
    GET_SAMPLING_PORT_CURRENT_STATUS (PORT1, &STATUS, &RC);
	
    /* EXPECTED: */
    if ((NO_ERROR == RC) && (PERIOD == STATUS.REFRESH_PERIOD) && 
	    (SIZE == STATUS.MAX_MESSAGE_SIZE) &&
		(DESTINATION == STATUS.PORT_DIRECTION) &&
	    (FRESH == STATUS.MESSAGE_AGE) && (CONSUMED_MESSAGE == STATUS.UPDATED) &&
		(0 < STATUS.TIME_STAMP) && (0 == unexp_error))  {
		
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 10 
    	Sleep for two mtfs. */
    test_step_announce(10,1);

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
	
	/* Sleep for two mtfs */
	rtems_task_wake_after(2*mtf_ticks);

    /* Test Step 11 
    	Get current status from the destination port. Verify the message is
	reported as stale and consumed. */
    test_step_announce(11,1);

    /* Test step 11 code */
    GET_SAMPLING_PORT_CURRENT_STATUS (PORT1, &STATUS, &RC);

    /* EXPECTED: */
    if ((NO_ERROR == RC) && (PERIOD == STATUS.REFRESH_PERIOD) && 
	    (SIZE == STATUS.MAX_MESSAGE_SIZE) &&
		(DESTINATION == STATUS.PORT_DIRECTION) &&
	    (STALE == STATUS.MESSAGE_AGE) && (CONSUMED_MESSAGE == STATUS.UPDATED) &&
		(0 < STATUS.TIME_STAMP) && (0 == unexp_error))  {
		
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 12 
    	Wait for next mtf */
    test_step_announce(12,1);

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
	
	/* wait for next mtf */
	rtems_task_wake_after(mtf_ticks);
	
    /* Test Step 14 
    	Get current status from the destination port. Verify the port has a
	new, fresh, message. */
    test_step_announce(14,1);

    /* Test step 14 code */
    GET_SAMPLING_PORT_CURRENT_STATUS (PORT1, &STATUS, &RC);
	
    /* EXPECTED: */
    if ((NO_ERROR == RC) && (PERIOD == STATUS.REFRESH_PERIOD) && 
	    (SIZE == STATUS.MAX_MESSAGE_SIZE) &&
		(DESTINATION == STATUS.PORT_DIRECTION) &&
	    (FRESH == STATUS.MESSAGE_AGE) && (NEW_MESSAGE == STATUS.UPDATED) &&
		(0 < STATUS.TIME_STAMP) && (0 == unexp_error))  {
		
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    
    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}


