
/* ***********************************************************************    */
/* ** TEST-DEF-01730 / 1730 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0690                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Create a test where one partition writes multicast messages to a sampling
	port, and assert that all receiving partitions receive the data that was sent. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P0testdef.h>
#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 50;

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
	SAMPLING_PORT_NAME_TYPE NAME = "A1_S";
	MESSAGE_SIZE_TYPE SIZE  = 32;
	SYSTEM_TIME_TYPE PERIOD = 2000000000;
	unsigned char msg[32] = "1 1 2 3 5 8 13 21 34 55 89 144\0\0";
	RETURN_CODE_TYPE RC;
	SAMPLING_PORT_ID_TYPE PORT;
                                        
    /* Test Start ******************************************************    */
    test_enter(1730);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Create Sampling Port */
    test_step_announce(0,1);

    CREATE_SAMPLING_PORT(NAME, SIZE, SOURCE, PERIOD, &PORT, &RC);
	
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

    /* Test Step 1 
    	Write dummy message to Sampling Port */
    test_step_announce(1,1);

	WRITE_SAMPLING_MESSAGE(PORT, msg, 32, &RC);

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


