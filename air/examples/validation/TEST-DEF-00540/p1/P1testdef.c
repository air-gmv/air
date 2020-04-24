
/* ***********************************************************************    */
/* ** TEST-DEF-00540 / 540 ********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0430                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Test if a read data access to an unaligned address is detected by the
	LEON, and the associated fault detected by the HM. Check that the remaining
	partitions are not affected. */

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

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
	/* signal error ocurrence	*/
    unexp_error = i_error;
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
	unsigned int my_int = 0;
	unsigned int *ptr;
	unsigned int aux = 0xDEAD;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 2 
    	Read from a unaligned int address (int shall be word aligned).
	Verify that a trap was raised */
    test_step_announce(2,1);

    my_int = (int) &aux;
	my_int++;
	
	ptr = (unsigned int *)my_int;
	aux = *ptr;
	
	/* Test step 2 code */
    ret |= RTEMS_SUCCESSFUL;

    /* EXPECTED: */
    if (PMK_SEG_ERR == unexp_error)  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
		unexp_error = 0;
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    unexp_error);
		test_exit(TEST_FAILURE,mtf_ticks>>1);
    }

    /* Test Step 3 
    	Wait for next execution window. */
    test_step_announce(3,1);

    /* Test step 3 code */
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
		test_exit(TEST_FAILURE,mtf_ticks>>1);
    }
	
	rtems_task_wake_after(mtf_ticks);

    
    /* Test End */
    test_return();
    return 0;
}


