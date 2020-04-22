
/* ***********************************************************************    */
/* ** TEST-DEF-80400 / 80400 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0340                                                            */
/*    AIR_SEP_0760                                                            */
/*    AIR_SEP_0930                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Create a test for the pmk_sparc_enable_cache and pmk_sparc_disable_cache
	system calls. Run the same code in a second non-authorized partition and check
	that a Health Monitor event is generated. */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P2testdef.h>
#include <pal_error.h>
#include <pal_system.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/
typedef enum {
	ALL_CACHES = 0, 		/**< Instruction and Data cache */
	DATA_CACHE = 1, 		/**< Data cache only */
	INSTRUCTION_CACHE = 2, 	/**< Instruction cache only */
} CACHE_TYPE;

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 160;

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
	unsigned int RC;
	unsigned int ccr;
	
    /* Test Steps *******************************************************    */
    /* Test Step 10 
    	Try to activate the instruction cache. Since it is already enabled
	at configuration level the return code shall be PAL_NO_ACTION. */
    test_step_announce(10,1);

    RC = pmk_sparc_enable_cache(INSTRUCTION_CACHE);

    /* EXPECTED: */
    if ((PAL_NO_ACTION == RC) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 11 
    	Try to deactivate the data cache. Since it is already deactivated at
	configuration level the return code shall be PAL_NO_ACTION. */
    test_step_announce(11,1);

    RC = pmk_sparc_disable_cache(DATA_CACHE);

    /* EXPECTED: */
    if ((PAL_NO_ACTION == RC) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 12 
    	Activate Both Caches. Action shall succeed with PAL_NO_ERROR. */
    test_step_announce(12,1);

    RC = pmk_sparc_enable_cache(ALL_CACHES);

    /* EXPECTED: */
    if ((PAL_NO_ERROR == RC) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	
    /* Test Step 13 
    	Read the cache control register. Verify that both caches are
	enabled. */
    test_step_announce(13,1);

     /* Test step 7 code */
    ccr = pmk_sparc_get_cache_ctrl();
	
        /* EXPECTED: */
	if (((ccr & 0xF) == 0xF) && (0 == unexp_error))  {
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

