
/* ***********************************************************************    */
/* ** TEST-DEF-00022 / 22 *********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0530                                                            */
/*    AIR_SEP_0540                                                            */
/*    AIR_SEP_0550                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define a test with two non-system partitions where one calls the
	SET_A_PARTITION_MODE service with the PARTITION_IDENTIFIER of the other. Check that
	INVALID_CONFIG is returned and the target partition mode remains unchanged. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P0testdef.h>

#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 60;

/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    
	/* signal error ocurrence	*/
    unexp_error	= 1;
    return;
}

/* Test execution *******************************************************    */
int test_main (void) {
    /* Test generic variables  ******************************************	*/
	/* repeatition iteration counter	*/ 
	int repeat     = 0;                
	/* total test result                */
    int res     = TEST_SUCCESS;     
    
    /* Test specific variables  ******************************************	*/
	
    RETURN_CODE_TYPE		rc 			= -1;
    int						test_err	= -1;
    PARTITION_ID_TYPE		invalid_id	= 99;
    PARTITION_ID_TYPE		P1id		= -1;

    NAME_TYPE				P1name		= "p1";
    PARTITION_STATUS_TYPE	target_status;

    /* Test Start ******************************************************    */
    test_enter(22);
    debug_libtest();
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	TSAL_INIT; set partition mode to NORMAL; expect NO_ERROR. */
    test_step_announce(0,1);


    /* Test step 0 code */

    test_err = 0;
    rc = TSAL_INIT();

    if (rc != NO_ERROR){
    	test_err = 1;
    }

    rc = -1;
    SET_PARTITION_MODE(NORMAL, &rc);

    /* EXPECTED: */
    if ((0 == test_err) && (NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    rc);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    rc);
    }
    rtems_task_wake_after(mtf_ticks);


    /* Test Step 2 
    	Attempt to call GET_A_PARTITION_STATUS using an invalid target
	partition id; expect INVALID_PARAM. */
    test_step_announce(2,1);

    /* Test step 2 code */

    rc = -1;
    GET_A_PARTITION_STATUS(invalid_id, &target_status, &rc);

    /* EXPECTED: INVALID_PARAM for using an invalid target partition id */
    if ((INVALID_PARAM == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    rc);
    } else {
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    rc);
    }

    /* Test Step 3 
    	Attempt to call SET_A_PARTITION_MODE using an invalid target
	partition id; expect INVALID_PARAM. */
    test_step_announce(3,1);

    /* Test step 3 code */

    rc = -1;
    SET_A_PARTITION_MODE(invalid_id, WARM_START, &rc);

    /* EXPECTED: INVALID_PARAM for using an invalid target partition id */
    if ((INVALID_PARAM == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    rc);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    rc);
    }

    /* Test Step 4 
    	Call GET_A_PARTITION_ID to obtain the id of partition P1; expect
	NO_ERROR. */
    test_step_announce(4,1);

    /* Test step 4 code */

    rc = -1;
    GET_A_PARTITION_ID(P1name, &P1id, &rc);

    /* EXPECTED: NO_ERROR */
    if ((NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    rc);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    rc);
    }

    /* Test Step 5 
    	Attempt to set partition P1 operating mode to an invalid operating
	mode (IDLE-1); expect INVALID_PARAM. */
    test_step_announce(5,1);

    /* Test step 5 code */

    rc = -1;
    SET_A_PARTITION_MODE(P1id, (IDLE-1), &rc);

    /* EXPECTED: INVALID_PARAM for using an invalid operating mode */
    if ((INVALID_PARAM == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    rc);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    rc);
    }

    /* Test Step 6 
    	Attempt to set partition P1 operating mode to an invalid operating
	mode (NORMAL+1); expect INVALID_PARAM. */
    test_step_announce(6,1);

    /* Test step 6 code */

    rc = -1;
    SET_A_PARTITION_MODE(P1id, (NORMAL+1), &rc);

    /* EXPECTED: INVALID_PARAM for using an invalid operating mode */
    if ((INVALID_PARAM == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    rc);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    rc);
    }

    /* Test Step 7 
    	Set partition P1 operating mode to a valid mode using a valid
	transition (NORMAL->WARM_START). */
    test_step_announce(7,1);

    /* Test step 7 code */

    rc = -1;
    SET_A_PARTITION_MODE(P1id, WARM_START, &rc);

    /* EXPECTED: NO_ERROR */
    if ((NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    rc);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    rc);
    }
    rtems_task_wake_after(mtf_ticks);


    /* Test Step 8 
    	Call GET_A_PARTITION_STATUS for partition P1; verify P1 operating
	mode is WARM_START; expect NO_ERROR. */
    test_step_announce(8,1);

    /* Test step 8 code */

    rc = -1;
    target_status.OPERATING_MODE = -1;

    GET_A_PARTITION_STATUS(P1id, &target_status, &rc);

    /* EXPECTED: NO_ERROR && P1 operating mode is WARM_START */
    if ((NO_ERROR == rc) 									&&
    		(WARM_START == target_status.OPERATING_MODE) 	&&
    		(0 == unexp_error))  {

        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    rc);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    rc);
    }

    rtems_task_wake_after(mtf_ticks);

    
    /* Test End */
    test_return(res);
    return 0;
}


