
/* ***********************************************************************    */
/* ** TEST-DEF-80060 / 80060 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0070                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Configure a test with 3 partitions with a plug-in where a shared memory
	area (with 4KB) has the following permissions: P0: RW P1: R P2: No Access Test
	if the application running in partition 0 can read and write data from that
	memory area, partition 1 can only read data and an attempt to write data
	generates a Health Monitor event, and P2 can neither read or write without raising
	an HM event. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
volatile int unexp_error	   = 0;
int mtf_ticks    	= 150;

/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    
	/* signal error ocurrence	*/
    unexp_error	+= 1;
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
	int * plugin_memarea = NULL;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 3 
    	Call get_usr_plugins_fst_memarea with the plugin type
	PMK_PLUGIN_TEST to obtain a pointer to the plug-in memory area. */
        
    test_step_announce(3,1);
    /* Test step 3 code */
	plugin_memarea = (int* ) get_usr_plugins_fst_memarea(PMK_PLUGIN_TEST);
    ret = plugin_memarea == NULL ? RTEMS_INVALID_ADDRESS : RTEMS_SUCCESSFUL;
 
    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 4 
    	Read the value of a integer at the start of the plug-in memory area.
	Check that the health monitor callback counter remains at zero. */
        
    test_step_announce(4,1);
    /* Test step 4 code */
	if (*plugin_memarea == -1) {
		ret = RTEMS_INTERNAL_ERROR;
	}
	if (unexp_error > 0) {
		ret = RTEMS_INTERNAL_ERROR;
	}

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 5 
    	Write an integer value at the start of the plug-in memory area.
	Check that the health monitor callback counter has increased to one. */
    test_step_announce(5,1);

    /* Test step 5 code */
	*plugin_memarea = 2;
	if (unexp_error != 1) {
		ret = RTEMS_INTERNAL_ERROR;
	} else {
		ret = RTEMS_SUCCESSFUL;
	}
 
    /* EXPECTED: */
    if (RTEMS_SUCCESSFUL == ret) {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    unexp_error);
    }

    /* Test End */
    test_return(res);
    return 0;
}

