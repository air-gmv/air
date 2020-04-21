/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system testsuite, which is distributed under the terms of the AIR License
 *  Agreement.
 *  The full extent of the AIR License Agreement is described in the contract
 *  celebrated between GMVIS Skysoft and the Client.
 * ==========================================================================*/

/*
 * TEST-DEF-00770
 *
 * Requirements:
 *
 * Test Description:
 *    Define a test configuration with the following actions for the given
 *    states: State: Partition Application, Error: Segmentation Error,
 *    ErrorLevel: Partition State: Health Monitor Processing, Error:
 *    Segmentation Error, ErrorLevel: Module The test application shall generate
 *    a segmentation fault. The partition callback called by the Health Monitor
 *    shall in turn also generate a segmentation fault. Check that this
 *    translates into a system level HM event that is properly detected and
 *    handled.
 */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P0testdef.h>

/* Test external definitions */


/* Test type definitions */


/* Test global variables */
int unexp_error	    = 0;
int mod_callback_control = 0;
int mtf_ticks    	= 30;


int *get_mod_callback_control (void) {
    return &(mod_callback_control);
}


/**
 *  @brief Test HM callbacks
 */
void hm_part_callback(pmk_hm_err_id_e i_error,void *i_state){

    /* i_state is not really a pointer, this signature is required due to the
       pal_callbacks function signature -> convert it into a relevant value */
    pmk_hm_state_id_e state = (pmk_hm_state_id_e) i_state;

	/* signal error occurrence	*/
    if (i_error == PMK_SEG_ERR) {
        unexp_error++;
        void *ptr = NULL;
        memcpy(ptr, &i_error, 1);
    }
}

/**
 *   @brief Test execution
 */
int test_main(void){

	/* repetition iteration counter	*/
	int repeat              = 0;
	/* function to test return code */
    rtems_status_code ret   = RTEMS_SUCCESSFUL;
	/* total test result */
    int res                 = TEST_SUCCESS;

    /* Test specific variables */


    /* Test Start */
    test_enter(770);

    /*
     * Test Step 0:
     *    Generate an PMK_SEG_ERR on the application (try to write to
     *    0x00000000)
     */
    test_step_announce(0, 1);

    void *ptr = NULL;
    /* random source value */
    uint8_t value = 7;
    
    /* write to 0x00000000 */
    memcpy(ptr, (void *) &value, 1);

    /* Test Step 0 code */
    ret |= RTEMS_SUCCESSFUL;

    /* Check if the result is expected */
    if ((RTEMS_SUCCESSFUL == ret) && (ptr == NULL)) {
        res &= test_step_report(TEST_SUCCESS,
                                RESULT_EQUAL | RESULT_TYPE_VALUE, ret);
    } else {
        res &= test_step_report(TEST_FAILURE,
                                RESULT_DIFF | RESULT_TYPE_VALUE, ret);
    }

    /*
     * Test Step 1:
     *    Check that the partition health monitor callback has been called
     */
    test_step_announce(1, 1);

    /* Test Step 1 code */
    ret |= RTEMS_SUCCESSFUL;

    /* Check if the result is expected */
    if ((RTEMS_SUCCESSFUL == ret) && (1 == unexp_error)) {
        res &= test_step_report(TEST_SUCCESS,
                                RESULT_EQUAL | RESULT_TYPE_VALUE, ret);
    } else {
        res &= test_step_report(TEST_FAILURE,
                                RESULT_DIFF | RESULT_TYPE_VALUE, ret);
    }

    /*
     * Test Step 2:
     *    Generate an PMK_SEG_ERR on the health monitor callback
     */
    test_step_announce(2, 1);

    /* Test Step 2 code */
    ret |= RTEMS_SUCCESSFUL;

    /* Check if the result is expected */
    if ((RTEMS_SUCCESSFUL == ret)
        && (NULL != mod_callback_control) ) {
        res &= test_step_report(TEST_SUCCESS,
                                RESULT_EQUAL | RESULT_TYPE_VALUE, ret);
    } else {
        res &= test_step_report(TEST_FAILURE,
                                RESULT_DIFF | RESULT_TYPE_VALUE, ret);
    }

    /*
     * Test Step 3:
     *    Check that the module health monitor callback has been called.
     */
    test_step_announce(3, 1);

    /* Test Step 3 code */
    ret |= RTEMS_SUCCESSFUL;

    /* Check if the result is expected */
    if ((RTEMS_SUCCESSFUL == ret) && (0 < mod_callback_control)) {
        res &= test_step_report(TEST_SUCCESS,
                                RESULT_EQUAL | RESULT_TYPE_VALUE, ret);
    } else {
        res &= test_step_report(TEST_FAILURE,
                                RESULT_DIFF | RESULT_TYPE_VALUE, ret);
    }

    /*
     * Test Step 4:
     *    Terminate test.
     */
    test_step_announce(4, 1);

    /* Test Step 4 code */
    ret |= RTEMS_SUCCESSFUL;

    /* Check if the result is expected */
    if ((RTEMS_SUCCESSFUL == ret) && (1 == mod_callback_control) &&
        (1 == unexp_error)) {
        res &= test_step_report(TEST_SUCCESS,
                                RESULT_EQUAL | RESULT_TYPE_VALUE, ret);
    } else {
        res &= test_step_report(TEST_FAILURE,
                                RESULT_DIFF | RESULT_TYPE_VALUE, ret);
    }

    /* Test End */
    test_exit(res, mtf_ticks);
    return 0;
}
