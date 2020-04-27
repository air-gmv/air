
/* ***********************************************************************    */
/* ** TEST-DEF-00023 / 23 *********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0555                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Devise a test with two partitions, where only the first is authorized to
	call GET_A_PARTITION_ID. In the first partition: Check that a call to
	GET_A_PARTITION_ID before a call to TSAL_INIT returns an INVALID_MODE error. Get the
	partition ID of the second partition, and check that it is correct. Check that the
	same call with an incorrect partition name returns a INVALID_PARAM error. In
	the second partition: Check that a call to GET_A_PARTITION_ID returns
	INVALID_CONFIG irrespectively of a correct partition name being passed or not. */

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
	/* function to test return code     */
    rtems_status_code ret    = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res     = TEST_SUCCESS;     
    
    /* Test specific variables  ******************************************	*/

    RETURN_CODE_TYPE 		rc 			= -1;
    PARTITION_ID_TYPE 		targetid	= -1;
	
    /* Test Start ******************************************************    */
    test_enter(23);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	GET_A_PARTITION_ID on partition name 'p1'; expect INVALID_MODE: TSAL
	not initialized. */
    test_step_announce(0,1);

    /* Test step 0 code */

	/* obtain id of the target partition given its name */
	GET_A_PARTITION_ID("p1", &targetid, &rc);

    /* EXPECTED: */
    if ((INVALID_MODE == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 1 
    	TSAL_INIT; expect NO_ERROR. */
    test_step_announce(1,1);

    /* Test step 1 code */

    rc = -1;
    rc = TSAL_INIT();

    /* EXPECTED: */
    if ((NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks);

    /* Test Step 3 
    	GET_A_PARTITION_ID on valid partition name 'p1'; expect NO_ERROR. */
    test_step_announce(3,1);

    /* Test step 3 code */

    rc = -1;

    /* obtain id of the target partition given its name */
    GET_A_PARTITION_ID("p1", &targetid, &rc);

    /* EXPECTED: */
    if ((NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 4 
    	GET_A_PARTITION_ID on invalid partition name 'XYZ'; expect
	INVALID_PARAM: name doesn't identify an existing partition. */
    test_step_announce(4,1);

    /* Test step 4 code */

    rc = -1;

	/* obtain id of the target partition given its name */
	GET_A_PARTITION_ID("XYZ", &targetid, &rc);

    /* EXPECTED: */
    if ((INVALID_PARAM == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
    
    rtems_task_wake_after(mtf_ticks);

    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}


