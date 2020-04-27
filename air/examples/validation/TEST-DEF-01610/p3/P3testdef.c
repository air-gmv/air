
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

#include <air.h>
#include <air_test.h>

#include <P3testdef.h>

#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   			= 0;
int mtf_ticks    			= 50;
pmk_hm_state_id_e 	hmstate = -1;
pmk_hm_err_id_e		hmerr	= -1;

/* counts the number of partition reboot since data section is preserved across restarts */
volatile int reboot_count = 0;

/* Test auxiliary functions    ********************************************	*/

static void SW_TRAP() {
	__asm__ __volatile__ ("ta 0x70");
}

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
	/* function to test return code     */
	rtems_status_code ret    = RTEMS_SUCCESSFUL;
	/* total test result                */
	int res     = TEST_SUCCESS;

    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    hmstate = (pmk_hm_state_id_e) 	i_state;
    hmerr	= (pmk_hm_err_id_e) 	i_error;
	/* signal error ocurrence	*/
    unexp_error	= 1;

    if (	(PMK_VIOL_ERR 	== hmerr) 	&&
    		(PMK_PAR_OS		== hmstate))  {
    	res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
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
	
    int 				test_err	= -1;
    RETURN_CODE_TYPE 	rc 			= -1;

    if(reboot_count > 0){
        rtems_task_wake_after(mtf_ticks);
        test_return();
    }
    reboot_count++;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 3 
    	TSAL_INIT; set partition mode to NORMAL; expect NO_ERROR. */
    test_step_announce(3,1);

    /* Test step 3 code */

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
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
    rtems_task_wake_after(mtf_ticks);


    /* Test Step 8 
    	Generate sw trap. Expect an Health Monitor event was generated with
	error PMK_VIOL_ERR. */
    test_step_announce(8,1);

    /* Test step 8 code */

    SW_TRAP();

    /* EXPECTED: */
//    if ((1 == unexp_error) 				&&
//    		(PMK_VIOL_ERR 	== hmerr) 	&&
//    		(PMK_PAR_OS		== hmstate))  {
//
//    	unexp_error = 0;
//    	res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
//                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
//                                    ret);
//
//    } else {
//        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
//                                    RESULT_DIFF | RESULT_TYPE_VALUE,
//                                    ret);
//    }
//    rtems_task_wake_after(mtf_ticks);
    
    /* Test End */
    test_return();
    return 0;
}


