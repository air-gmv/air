
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
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P2testdef.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 150;

/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void hm_part_callback (pmk_hm_err_id_e i_error,void *i_state) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    pmk_hm_state_id_e state = (pmk_hm_state_id_e) i_state;
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
    /* Test Step 6 
    	Call get_usr_plugins_fst_memarea with the plugin type
	PMK_PLUGIN_TEST to obtain a pointer to the plug-in memory area. */
    test_step_announce(6,1);
    /* Test step 6 code */
	plugin_memarea = (int* ) get_usr_plugins_fst_memarea(PMK_PLUGIN_TEST);
    ret = plugin_memarea == NULL ? RTEMS_INVALID_ADDRESS : RTEMS_SUCCESSFUL;

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

    /* Test Step 7 
    	Read the value of a integer at the start of the plug-in memory area.
	Check that the health monitor callback counter has increased to one. */
    test_step_announce(7,1);

    /* Test step 7 code */
	if (*plugin_memarea == -1) {
		ret = RTEMS_SUCCESSFUL;
	}
	if (unexp_error != 1) {
		ret = RTEMS_INTERNAL_ERROR;
	}


    /* EXPECTED: */
    if (RTEMS_SUCCESSFUL == ret) {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 8 
    	Write an integer value at the start of the plug-in memory area.
	Check that the health monitor callback counter has increased to two. */
        
    test_step_announce(8,1);
    /* Test step 8 code */
	*plugin_memarea = 2;
	if (unexp_error != 2) {
		ret = RTEMS_INTERNAL_ERROR;
	}
 

    /* EXPECTED: */
    if (RTEMS_SUCCESSFUL == ret)  {
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

