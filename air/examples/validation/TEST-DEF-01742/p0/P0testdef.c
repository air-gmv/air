
/* ***********************************************************************    */
/* ** TEST-DEF-01742 / 1742 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0700                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Create a unitary test for the READ_SAMPLING_MESSAGE_CONDITIONAL function. */

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
	SAMPLING_PORT_ID_TYPE PORT0;
    SAMPLING_PORT_ID_TYPE PORT1;
	SAMPLING_PORT_NAME_TYPE NAME0 = "A1_S";
	SAMPLING_PORT_NAME_TYPE NAME1 = "B1_S";
	MESSAGE_SIZE_TYPE SIZE = 4;
	SYSTEM_TIME_TYPE PERIOD = 2000000000;
	SYSTEM_TIME_TYPE REF = 200000000;
	SYSTEM_TIME_TYPE REF_SUP = 2000000000;
	SYSTEM_TIME_TYPE STAMP;
	RETURN_CODE_TYPE RC;
	int error = 0;
	MESSAGE_SIZE_TYPE IN_SIZE;
	unsigned char BUF[8];
	
    /* Test Start ******************************************************    */
    test_enter(1742);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Call READ_SAMPLING_MESSAGE_CONDITIONAL with an invalid port id
	(Invalid Param). */
    test_step_announce(0,1);

    /* Test step 0 code */
    READ_SAMPLING_MESSAGE_CONDITIONAL(PORT0, REF, &BUF[0], &IN_SIZE, &STAMP, &RC);

    /* EXPECTED: */
    if ((INVALID_PARAM == RC) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
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
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 2 
    	READ_SAMPLING_MESSAGE_CONDITIONAL from the SOURCE port (Invalid
	Port). */
    test_step_announce(2,1);

    /* Test step 2 code */
    READ_SAMPLING_MESSAGE_CONDITIONAL(PORT0, REF, &BUF[0], &IN_SIZE, &STAMP, &RC);

    /* EXPECTED: */
    if ((INVALID_MODE == RC) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 3 
    	READ_SAMPLING_MESSAGE_CONDITIONAL with an invalid REF_TIME_STAMP */
    test_step_announce(3,1);

    /* Test step 3 code */
    READ_SAMPLING_MESSAGE_CONDITIONAL(PORT1, -1, &BUF[0], &IN_SIZE, &STAMP, &RC);

    /* EXPECTED: */
    if ((INVALID_PARAM == RC) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 4 
    	READ_SAMPLING_MESSAGE_CONDITIONAL with an NULL MESSAGE_ADDR. */
    test_step_announce(4,1);

    /* Test step 4 code */
    READ_SAMPLING_MESSAGE_CONDITIONAL(PORT1, REF, NULL, &IN_SIZE, &STAMP, &RC);

    /* EXPECTED: */
    if ((INVALID_PARAM == RC) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 5 
    	READ_SAMPLING_MESSAGE_CONDITIONAL from the DESTINATION port
	(Currently empty: No action). */
    test_step_announce(5,1);

    /* Test step 5 code */
    READ_SAMPLING_MESSAGE_CONDITIONAL(PORT1, REF, &BUF[0], &IN_SIZE, &STAMP, &RC);

    /* EXPECTED: */
    if ((0 == STAMP) && (0 == IN_SIZE) && 
		(NO_ACTION == RC) && (0 == unexp_error)){
		
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 6 
    	Wait for next mtf. */
    test_step_announce(6,1);

    /* EXPECTED: */
    if (0 == unexp_error) {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	
	rtems_task_wake_after(mtf_ticks);

    /* Test Step 9 
    	READ_SAMPLING_MESSAGE_CONDITIONAL from the DESTINATION port (No
	error). */
    test_step_announce(9,1);

    /* Test step 9 code */
    READ_SAMPLING_MESSAGE_CONDITIONAL(PORT1, REF, &BUF[0], &IN_SIZE, &STAMP, &RC);

    /* EXPECTED: */
    if ((0 < STAMP) && (4 == IN_SIZE) && 
		(NO_ERROR == RC) && (0 == unexp_error)){
		
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 10 
    	READ_SAMPLING_MESSAGE_CONDITIONAL from the DESTINATION port with an
	invalid REF_TIME_STAMP. (No Action) */
    test_step_announce(10,1);

    /* Test step 10 code */
    READ_SAMPLING_MESSAGE_CONDITIONAL(PORT1, REF_SUP, &BUF[0], &IN_SIZE, &STAMP, &RC);

    /* EXPECTED: */
    if ((0 < STAMP) && (0 == IN_SIZE) && 
		(NO_ACTION == RC) && (0 == unexp_error)){
		
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    
    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}


