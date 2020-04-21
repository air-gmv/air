
/* ***********************************************************************    */
/* ** TEST-DEF-01390 / 1390 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0560                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	AIR SEP 0560 "During the POS boot-up the partition mode shall be either
	COLD-START or WARM-START. If the system is starting (or rebooting) the mode shall be
	COLD-START. The partition is able to determine which through the GET_PARTITION_STATUS
	service." TEST-DEF Devise a test with three partitions, where the third is not
	included in an initial schedule. The second module schedule shall only assign one
	tick to the third partition, and the number of ticks per second shall be small
	(e.g. 1 ms/tick). Check that a call to GET_PARTITION_STATUS on the newly run
	partition (after a SET_MODULE_SCHEDULE function call) returns the status COLD_START. */

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

    RETURN_CODE_TYPE 		rc = -1;
    PARTITION_STATUS_TYPE 	status;

    /* Test Steps *******************************************************    */
    /* Test Step 12
    	Verify partition mode is the initial mode (COLD_START). */
    test_step_announce(12,1);

    /* Test step 11 code */
	/* clean up the variables to be verified */
    rc 						= -1;
	status.OPERATING_MODE 	= -1;

	/* obtain mode in partition status */
    GET_PARTITION_STATUS(&status, &rc);

    /* EXPECTED: NO_ERROR, operating mode == COLD_START */
    if ((NO_ERROR == rc) && (status.OPERATING_MODE == COLD_START) && (0 == unexp_error))  {
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


