
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

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>

#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 100;

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
	SAMPLING_PORT_NAME_TYPE NAME0 = "A2_S";
	SAMPLING_PORT_NAME_TYPE NAME1 = "B2_S";
	MESSAGE_SIZE_TYPE SIZE = 4;
	SYSTEM_TIME_TYPE PERIOD = 2000000000;
	unsigned char msg0[8] = "OLA\0";
	RETURN_CODE_TYPE RC;
	int error = 0;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 5 
    	Create two sampling ports with distinct directions (no error
	expected). */
    test_step_announce(5,1);

    /* Test step 5 code */
    CREATE_SAMPLING_PORT (NAME0, SIZE, DESTINATION, PERIOD, &PORT0, &RC);
	if(RC != NO_ERROR){
		error = 1;
	}

	CREATE_SAMPLING_PORT (NAME1, SIZE, SOURCE, PERIOD, &PORT1, &RC);
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

    /* Test Step 6 
    	Write to source sampling port (No error) */
    test_step_announce(6,1);

    /* Test step 6 code */
    WRITE_SAMPLING_MESSAGE(PORT1, msg0, 4, &RC);

    /* EXPECTED: */
    if ((NO_ERROR == RC) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 7 
    	Sleep for three mtfs. */
    test_step_announce(7,1);

    /* EXPECTED: */
    if(0 == unexp_error) {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	/* Sleep for three mtfs. */
	rtems_task_wake_after(3*mtf_ticks);
	
    /* Test Step 13 
    	Write to source sampling port (No error) */
    test_step_announce(13,1);

    /* Test step 13 code */
    WRITE_SAMPLING_MESSAGE(PORT1, msg0, 4, &RC);

    /* EXPECTED: */
    if ((NO_ERROR == RC) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    
    /* Test End */
    test_return();
    return 0;
}


