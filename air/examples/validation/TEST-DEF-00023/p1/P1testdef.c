
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
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P1testdef.h>

#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 60;

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
	
    RETURN_CODE_TYPE 		rc 			= -1;
    PARTITION_ID_TYPE 		targetid	= -1;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 2 
    	TSAL_INIT; expect NO_ERROR. */
    test_step_announce(2,1);

    /* Test step 2 code */
    rc = TSAL_INIT();

    /* EXPECTED: */
    if ((NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks);

    /* Test Step 5 
    	GET_A_PARTITION_ID on valid partition name 'p0'; expect
	INVALID_CONFIG: calling partition is not a system partition. */
    test_step_announce(5,1);

    /* Test step 5 code */

    rc = -1;

    /* obtain id of the target partition given its name */
    GET_A_PARTITION_ID("p0", &targetid, &rc);

    /* EXPECTED: */
    if ((INVALID_CONFIG == rc) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 6 
    	GET_A_PARTITION_ID on invalid partition name 'XYZ'; expect
	INVALID_CONFIG: calling partition is not a system partition. */
    test_step_announce(6,1);

    /* Test step 6 code */
    rc = -1;

    /* obtain id of the target partition given its name */
    GET_A_PARTITION_ID("XYZ", &targetid, &rc);

    /* EXPECTED: */
    if ((INVALID_CONFIG == rc) && (0 == unexp_error))  {
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

