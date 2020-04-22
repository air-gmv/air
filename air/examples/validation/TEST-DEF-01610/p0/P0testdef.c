
/* ***********************************************************************    */
/* ** TEST-DEF-01610 / 1610 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0430                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define a system with three (or more) partitions where the same software
	trap in each partition raises a distinct recovery action: P0 : ignore P1 :
	partition warm start P2 : partition cold start Check that the configured action is
	properly executed in each partition. */

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
int unexp_error	   			= 0;
int mtf_ticks    			= 50;
pmk_hm_state_id_e 	hmstate = -1;
pmk_hm_err_id_e		hmerr	= -1;

/* Test auxiliary functions    ********************************************	*/

static void SW_TRAP() {
	__asm__ __volatile__ ("ta 0x70");
}

/* Test HM callbacks        ***********************************************	*/
void hm_part_callback (pmk_hm_err_id_e i_error,void *i_state) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    hmstate = (pmk_hm_state_id_e) 	i_state;
    hmerr	= (pmk_hm_err_id_e) 	i_error;
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
	
    int 					test_err	= -1;
    RETURN_CODE_TYPE 		rc 			= -1;
    PARTITION_ID_TYPE 		p1id 		= 1;
    PARTITION_ID_TYPE 		p2id 		= 2;
    PARTITION_ID_TYPE 		p3id 		= 3;

    PARTITION_STATUS_TYPE	part_status;

    /* Test Start ******************************************************    */
    test_enter(1610);
                                        
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

    /* EXPECTED: NO_ERROR */
    if ((0 == test_err) && (NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
    rtems_task_wake_after(mtf_ticks);

    /* Test Step 4 
    	Generate sw trap. Expect an Health Monitor event was generated with
	error PMK_VIOL_ERR. */
    test_step_announce(4,1);

    /* Test step 4 code */

    SW_TRAP();

    /* EXPECTED: */
    if (	(1 == unexp_error) 			&&
    		(PMK_VIOL_ERR 	== hmerr) 	&&
    		(PMK_PAR_OS		== hmstate))  {

    	unexp_error = 0;
    	res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);

    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }


    /* Test Step 5 
    	Obtain partition operating mode (via GET_PARTITION_STATUS); verify
	OPERATING_MODE == NORMAL. */
    test_step_announce(5,1);

    /* Test step 5 code */
    rc 							= -1;
    part_status.OPERATING_MODE 	= -1;

    GET_PARTITION_STATUS(&part_status, &rc);

    /* EXPECTED: */
    if (	(NO_ERROR == rc) 						&&
    		(part_status.OPERATING_MODE == NORMAL) 	&&
    		(0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
    rtems_task_wake_after(2*mtf_ticks);


    /* Test Step 9 
    	Obtain partition operating mode of P1 (via GET_A_PARTITION_STATUS);
	verify OPERATING_MODE == WARM_START. */
    test_step_announce(9,1);

    /* Test step 9 code */

    rc 							= -1;
    part_status.OPERATING_MODE 	= -1;

    GET_A_PARTITION_STATUS(p1id, &part_status, &rc);

    /* EXPECTED: */
    if (	(NO_ERROR == rc) 							&&
    		(part_status.OPERATING_MODE == WARM_START) 	&&
    		(0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 10 
    	Obtain partition operating mode of P2 (via GET_A_PARTITION_STATUS);
	verify OPERATING_MODE == COLD_START. */
    test_step_announce(10,1);

    /* Test step 10 code */

    rc 							= -1;
    part_status.OPERATING_MODE 	= -1;

    GET_A_PARTITION_STATUS(p2id, &part_status, &rc);

    /* EXPECTED: */
    if (	(NO_ERROR == rc) 							&&
    		(part_status.OPERATING_MODE == COLD_START) 	&&
    		(0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 11 
    	Obtain partition operating mode of P3 (via GET_A_PARTITION_STATUS);
	verify OPERATING_MODE == IDLE. */
    test_step_announce(11,1);

    /* Test step 11 code */

    rc 							= -1;
    part_status.OPERATING_MODE 	= -1;

    GET_A_PARTITION_STATUS(p3id, &part_status, &rc);

    /* EXPECTED: */
    if (	(NO_ERROR == rc) 						&&
    		(part_status.OPERATING_MODE == IDLE) 	&&
    		(0 == unexp_error))  {
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


