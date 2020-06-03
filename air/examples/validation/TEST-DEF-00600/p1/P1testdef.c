
/* ***********************************************************************    */
/* ** TEST-DEF-00600 / 600 ********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0300                                                            */
/*    AIR_SEP_0310                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define a test with two partitions, an application partition with
	non-supervisor permissions, and a system partition, with supervisor permission. Test if
	the execution of a supervisor mode instruction (read psr contents) in the
	application partition is captured and handled by the HM, and the associated HM action
	is called. Test if the remaining partitions continue to execute. Repeat the
	test in the system partition and check that no HM event is raised. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 30;

/* Test auxiliary functions    ********************************************	*/

//* read the processor status register */
void read_psr_register(void)
{
    __asm__ __volatile__ (" rd %psr,%g0 ");

}


/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    
	/* signal error ocurrence	*/
    unexp_error	= i_error;
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
	unsigned int i=0;
    /* Test Start ******************************************************    */
    test_enter(600);
    
    /* Test Steps *******************************************************    */
    /* Test Step 2 
    	Execute a regular operation. */
    test_step_announce(2,1);

    /* Test step 2 code */
    ret |= RTEMS_SUCCESSFUL;

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    	test_exit(TEST_FAILURE,mtf_ticks);
    }

    /* Test Step 3 
    	Execute a supervisor instruction [e.g. lda instruction]. Verify that
	the trap is raised. */
    test_step_announce(3,1);

    /* Test step 3 code */
	read_psr_register();

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (AIR_VIOLATION_ERROR == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
		unexp_error = 0;
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    unexp_error);
    	test_exit(TEST_FAILURE,mtf_ticks);
    }

    /* Test Step 4 
    	idle the partition */
    test_step_announce(4,1);

    /* Test step 4 code */
    ret |= RTEMS_SUCCESSFUL;

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    	test_exit(TEST_FAILURE,mtf_ticks);
    }

    
    /* Test End */
    test_return(res);
    return 0;
}


