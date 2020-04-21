
/* ***********************************************************************    */
/* ** TEST-DEF-01740 / 1740 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0700                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Create a unitary test for the READ_UPDATED_SAMPLING_MESSAGE function. */

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
int mtf_ticks    	= 30;

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
	SYSTEM_TIME_TYPE PERIOD = 2000000000;
	RETURN_CODE_TYPE RC;
	int error = 0;
	MESSAGE_SIZE_TYPE IN_SIZE;
	unsigned char BUF[8];
	UPDATED_TYPE UPDATED;
	
    /* Test Start ******************************************************    */
    test_enter(1740);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	READ_UPDATED_SAMPLING_MESSAGE with an invalid id (Invalid Param). */
    test_step_announce(0,1);

    /* Test step 0 code */
    READ_UPDATED_SAMPLING_MESSAGE(PORT1, (MESSAGE_ADDR_TYPE)&BUF, &IN_SIZE, &UPDATED, &RC);

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
    	READ_UPDATED_SAMPLING_MESSAGE from the SOURCE port (Invalid Port). */
    test_step_announce(2,1);

    /* Test step 2 code */
    READ_UPDATED_SAMPLING_MESSAGE(PORT0, (MESSAGE_ADDR_TYPE)&BUF, &IN_SIZE, &UPDATED, &RC);

    /* EXPECTED: */
    if ((INVALID_MODE == RC) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 3 
    	READ_UPDATED_SAMPLING_MESSAGE from the DESTINATION port (Currently
	empty: No action). */
    test_step_announce(3,1);

    /* Test step 3 code */
    READ_UPDATED_SAMPLING_MESSAGE(PORT1, (MESSAGE_ADDR_TYPE)&BUF, &IN_SIZE, &UPDATED, &RC);

    /* EXPECTED: */
    if ((NO_ACTION == RC) && (0 == unexp_error) && (EMPTY_PORT == UPDATED))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 4 
    	Wait for next mtf. */
    test_step_announce(4,1);

    /* Test step 4 code */

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
	
	rtems_task_wake_after(mtf_ticks);
	
    /* Test Step 7 
    	READ_UPDATED_SAMPLING_MESSAGE from the DESTINATION port (No error). */
    test_step_announce(7,1);

    /* Test step 7 code */
    READ_UPDATED_SAMPLING_MESSAGE(PORT1, (MESSAGE_ADDR_TYPE)&BUF, &IN_SIZE, &UPDATED, &RC);

    /* EXPECTED: */
    if ((NO_ERROR == RC) && (0 == unexp_error) && (NEW_MESSAGE == UPDATED))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 8 
    	READ_UPDATED_SAMPLING_MESSAGE from the DESTINATION port (Message was
	already consumed: no action). */
    test_step_announce(8,1);

    /* Test step 8 code */
    READ_UPDATED_SAMPLING_MESSAGE(PORT1, (MESSAGE_ADDR_TYPE)&BUF, &IN_SIZE, &UPDATED, &RC);

    /* EXPECTED: */
    if ((NO_ACTION == RC) && (0 == unexp_error) && (CONSUMED_MESSAGE == UPDATED))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    
    /* Test End */
	rtems_task_wake_after(mtf_ticks);
    test_exit(res,mtf_ticks);
    return 0;
}


