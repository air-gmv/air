
/* ***********************************************************************    */
/* ** TEST-DEF-00021 / 21 *********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0530                                                            */
/*    AIR_SEP_0540                                                            */
/*    AIR_SEP_0550                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define a system with four partitions: * The first partition is a system
	partition, capable of calling SET_A_PARTITION_MODE to set the mode of another
	partition; this partition uses GET_A_PARTITION_STATUS to perform verification by
	checking the status of the target partitions. * Secondary (target) partitions are
	user partitions, where the mode changes will be applied; The test shall call
	SET_A_PARTITION_MODE in the first partition for each of the valid mode transitions indicated in
	AIR_SEP_0530 with the PARTITION_IDENTIFIER set to the target partitions. Invalid
	transitions (N/A) transitions are also tested for errors. On the following cycle the
	first partition shall check that a mode was either effectively changed or
	remained unchanged (for the error cases). */

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
int mtf_ticks    	= 100;

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
    RETURN_CODE_TYPE 		rc = -1;

    /* status type for partitions p1, p2, p3 */
    PARTITION_STATUS_TYPE 	target_status[3];

    /* ids for partitions p1, p2, p3 */
    PARTITION_ID_TYPE target_ids[] = {1, 2, 3};

    /* detect unexpected return codes */
    int test_error = 0;

    /* aux iterator */
    int i = 0;
	
    /* Test Start ******************************************************    */
    test_enter(21);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Attempt to set mode of target partitions to COLD_START; expected
	INVALID_MODE for not initialized "TSAL". */
    test_step_announce(0,1);

    /* Test step 0 code */

    for (i=0; i<3; i++){
        SET_A_PARTITION_MODE(target_ids[i], COLD_START, &rc);
        if(INVALID_MODE != rc){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 1 
    	Initialize P0 TSAL; */
    test_step_announce(1,1);

    /* Test step 1 code */

    rc			= -1;
    test_error 	= 0;

	rc = TSAL_INIT();

    /* EXPECTED: */
    if ((NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 2 
    	Verify that the target partitions mode is the initial mode
	(COLD_START). */
    test_step_announce(2,1);

    /* Test step 2 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode COLD_START */
        if(target_status[i].OPERATING_MODE != COLD_START){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks);

    /* Test Step 6 
    	Attempt to set mode of target partitions to WARM_START; invalid
	trans: COLD_START->WARM_START. */
    test_step_announce(6,1);

    /* Test step 6 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* set mode of target partitions to WARM_START */
        SET_A_PARTITION_MODE(target_ids[i], WARM_START, &rc);

        /* all operations should return invalid target mode */
        if(INVALID_MODE != rc){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 7 
    	Verify that the target partitions mode is the initial mode
	(COLD_START). */
    test_step_announce(7,1);

    /* Test step 7 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode COLD_START */
        if(target_status[i].OPERATING_MODE != COLD_START){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 8 
    	Set mode of target partitions to COLD_START; trans:
	COLD_START->COLD_START. */
    test_step_announce(8,1);

    /* Test step 8 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* set mode of target partitions to COLD_START */
        SET_A_PARTITION_MODE(target_ids[i], COLD_START, &rc);

        /* all operations should return NO_ERROR */
        if(NO_ERROR != rc){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks);

    /* Test Step 9 
    	Verify that the target partitions mode is COLD_START. */
    test_step_announce(9,1);

    /* Test step 9 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode COLD_START */
        if(target_status[i].OPERATING_MODE != COLD_START){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 10 
    	Set mode of target partitions to normal; transition cold->normal is
	available only to the running partition via SET_PARTITION_MODE. invalid trans:
	COLD_START->NORMAL */
    test_step_announce(10,1);

    /* Test step 10 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* set mode of target partitions to COLD_START */
        SET_A_PARTITION_MODE(target_ids[i], NORMAL, &rc);

        /* all operations should return INVALID_MODE */
        if(INVALID_MODE != rc){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 11 
    	Verify that the target partitions mode is COLD_START. */
    test_step_announce(11,1);

    /* Test step 11 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode COLD_START */
        if(target_status[i].OPERATING_MODE != COLD_START){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks);

    /* Test Step 15 
    	Verify that the target partitions mode is NORMAL. */
    test_step_announce(15,1);

    /* Test step 15 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode COLD_START */
        if(target_status[i].OPERATING_MODE != NORMAL){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 16 
    	Set mode of target partitions to NORMAL; invalid trans:
	NORMAL->NORMAL. */
    test_step_announce(16,1);

    /* Test step 16 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* set mode of target partitions to COLD_START */
        SET_A_PARTITION_MODE(target_ids[i], NORMAL, &rc);

        /* all operations should return INVALID_MODE */
        if(INVALID_MODE != rc){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 17 
    	Verify that the target partitions mode is NORMAL. */
    test_step_announce(17,1);

    /* Test step 17 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode NORMAL */
        if(target_status[i].OPERATING_MODE != NORMAL){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 18 
    	Set mode of target partitions to WARM_START; trans:
	NORMAL->WARM_START. */
    test_step_announce(18,1);

    /* Test step 18 code */

    rc 			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* set mode of target partitions to WARM_START */
        SET_A_PARTITION_MODE(target_ids[i], WARM_START, &rc);

        /* all operations should return NO_ERROR */
        if(NO_ERROR != rc){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks);

    /* Test Step 19 
    	Verify that the target partitions mode is WARM_START. */
    test_step_announce(19,1);

    /* Test step 19 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode WARM_START */
        if(target_status[i].OPERATING_MODE != WARM_START){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 20 
    	Set mode of target partitions to WARM_START; trans:
	WARM_START->WARM_START. */
    test_step_announce(20,1);

    /* Test step 20 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

     	/* set mode of target partitions to WARM_START */
         SET_A_PARTITION_MODE(target_ids[i], WARM_START, &rc);

         /* all operations should return NO_ERROR */
         if(NO_ERROR != rc){
         	test_error = 1;
         }
     }

     /* EXPECTED: */
     if ((0 == test_error) && (0 == unexp_error))  {
         res &= test_step_report(    TEST_SUCCESS,
                                     RESULT_EQUAL | RESULT_TYPE_VALUE,
                                     ret);
     } else {
         res &= test_step_report(    TEST_FAILURE,
                                     RESULT_DIFF | RESULT_TYPE_VALUE,
                                     ret);
     }

     rtems_task_wake_after(mtf_ticks);

    /* Test Step 21 
    	Verify that the target partitions mode is WARM_START. */
    test_step_announce(21,1);

    /* Test step 21 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode WARM_START */
        if(target_status[i].OPERATING_MODE != WARM_START){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 22 
    	Set mode of target partitions to normal; transition warm->normal is
	available only to the running partition via SET_PARTITION_MODE. invalid trans:
	WARM_START->NORMAL */
    test_step_announce(22,1);

    /* Test step 22 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

     	/* set mode of target partitions to NORMAL */
         SET_A_PARTITION_MODE(target_ids[i], NORMAL, &rc);

         /* all operations should return NO_ERROR */
         if(INVALID_MODE != rc){
         	test_error = 1;
         }
     }

     /* EXPECTED: */
     if ((0 == test_error) && (0 == unexp_error))  {
         res &= test_step_report(    TEST_SUCCESS,
                                     RESULT_EQUAL | RESULT_TYPE_VALUE,
                                     ret);
     } else {
         res &= test_step_report(    TEST_FAILURE,
                                     RESULT_DIFF | RESULT_TYPE_VALUE,
                                     ret);
     }

    /* Test Step 23 
    	Verify that the target partitions mode is WARM_START. */
    test_step_announce(23,1);

    /* Test step 23 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode WARM_START */
        if(target_status[i].OPERATING_MODE != WARM_START){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks);

    /* Test Step 27 
    	Verify that the target partitions mode is NORMAL. */
    test_step_announce(27,1);

    /* Test step 27 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode NORMAL */
        if(target_status[i].OPERATING_MODE != NORMAL){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 28 
    	Set mode of target partitions to COLD_START; trans:
	NORMAL->COLD_START. */
    test_step_announce(28,1);

    /* Test step 28 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

     	/* set mode of target partitions to COLD_START */
         SET_A_PARTITION_MODE(target_ids[i], COLD_START, &rc);

         /* all operations should return NO_ERROR */
         if(NO_ERROR != rc){
         	test_error = 1;
         }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
    	res &= test_step_report(    TEST_SUCCESS,
    			RESULT_EQUAL | RESULT_TYPE_VALUE,
    			ret);
    } else {
    	res &= test_step_report(    TEST_FAILURE,
    			RESULT_DIFF | RESULT_TYPE_VALUE,
    			ret);
    }

    rtems_task_wake_after(mtf_ticks);

    /* Test Step 29 
    	Verify that the target partitions mode is COLD_START. */
    test_step_announce(29,1);

    /* Test step 29 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode COLD_START */
        if(target_status[i].OPERATING_MODE != COLD_START){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks);

    /* Test Step 33 
    	Verify that the target partitions mode is NORMAL. */
    test_step_announce(33,1);

    /* Test step 33 code */
    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode NORMAL */
        if(target_status[i].OPERATING_MODE != NORMAL){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 34 
    	Set mode of target partitions to WARM_START; trans:
	NORMAL->WARM_START (repeated). */
    test_step_announce(34,1);

    /* Test step 34 code */
    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

     	/* set mode of target partitions to WARM_START */
         SET_A_PARTITION_MODE(target_ids[i], WARM_START, &rc);

         /* all operations should return NO_ERROR */
         if(NO_ERROR != rc){
         	test_error = 1;
         }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
    	res &= test_step_report(    TEST_SUCCESS,
    			RESULT_EQUAL | RESULT_TYPE_VALUE,
    			ret);
    } else {
    	res &= test_step_report(    TEST_FAILURE,
    			RESULT_DIFF | RESULT_TYPE_VALUE,
    			ret);
    }

    rtems_task_wake_after(mtf_ticks);

    /* Test Step 35 
    	Verify that the target partitions mode is WARM_START. */
    test_step_announce(35,1);

    /* Test step 35 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode WARM_START */
        if(target_status[i].OPERATING_MODE != WARM_START){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 36 
    	Set mode of target partitions to COLD_START; trans:
	WARM_START->COLD_START. */
    test_step_announce(36,1);

    /* Test step 36 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* set mode of target partitions to COLD_START */
    	SET_A_PARTITION_MODE(target_ids[i], COLD_START, &rc);

    	/* all operations should return NO_ERROR */
    	if(NO_ERROR != rc){
    		test_error = 1;
    	}
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
    	res &= test_step_report(    TEST_SUCCESS,
    			RESULT_EQUAL | RESULT_TYPE_VALUE,
    			ret);
    } else {
    	res &= test_step_report(    TEST_FAILURE,
    			RESULT_DIFF | RESULT_TYPE_VALUE,
    			ret);
    }

    rtems_task_wake_after(mtf_ticks);

    /* Test Step 37 
    	Verify that the target partitions mode is COLD_START. */
    test_step_announce(37,1);

    /* Test step 37 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode COLD_START */
        if(target_status[i].OPERATING_MODE != COLD_START){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks);

    /* Test Step 40 
    	Verify that the target partitions mode is NORMAL for partitions p1
	and p2; COLD_START for partition p3. */
    test_step_announce(40,1);

    /* Test step 40 code */

    rc			= -1;
    test_error 	= 0;

    for (i=0; i<2; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions p1 and p2*/
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* partitions p1 and p2 should be in mode NORMAL */
        if(target_status[i].OPERATING_MODE != NORMAL){
        	test_error = 1;
        }
    }

	/* first clean up the variable to be verified*/
    target_status[2].OPERATING_MODE = -1;

	/* obtain mode in partition status of target partition p3 */
    GET_A_PARTITION_STATUS(target_ids[2], &target_status[2], &rc);

    /* partition p3 should be in mode COLD_START */
    if(target_status[2].OPERATING_MODE != COLD_START){
    	test_error = 1;
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 41 
    	Set up the transitions to idle -- 3: Set p2 partition mode to
	WARM_START; (Three target partitions are now in NORMAL (P1), WARM_START (P2),
	COLD_START (P3) trans: NORMAL->WARM_START (repeated). */
    test_step_announce(41,1);

    /* Test step 41 code */
    rc			= -1;
    test_error 	= 0;

    /* set mode of target partition p2 to WARM_START */
    SET_A_PARTITION_MODE(target_ids[1], WARM_START, &rc);

    /* all operations should return NO_ERROR */
    if(NO_ERROR != rc){
    	test_error = 1;
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
    	res &= test_step_report(    TEST_SUCCESS,
    			RESULT_EQUAL | RESULT_TYPE_VALUE,
    			ret);
    } else {
    	res &= test_step_report(    TEST_FAILURE,
    			RESULT_DIFF | RESULT_TYPE_VALUE,
    			ret);
    }

    rtems_task_wake_after(mtf_ticks);

    /* Test Step 42 
    	Verify that the target partitions mode is NORMAL for partition P1;
	WARM_START for partition P2; COLD_START for partition P3. */
    test_step_announce(42,1);

    /* Test step 42 code */
    rc			= -1;
    test_error 	= 0;

    /* verify p1 */
	/* first clean up the variable to be verified*/
    target_status[0].OPERATING_MODE = -1;

	/* obtain mode in partition status of target partition p3 */
    GET_A_PARTITION_STATUS(target_ids[0], &target_status[0], &rc);

    /* partition p1 should be in mode NORMAL */
    if(target_status[0].OPERATING_MODE != NORMAL){
    	test_error = 1;
    }

    /* verify p2 */
	/* first clean up the variable to be verified*/
    target_status[1].OPERATING_MODE = -1;

	/* obtain mode in partition status of target partition p3 */
    GET_A_PARTITION_STATUS(target_ids[1], &target_status[1], &rc);

    /* partition p2 should be in mode WARM_START */
    if(target_status[1].OPERATING_MODE != WARM_START){
    	test_error = 1;
    }

    /* verify p3 */
	/* first clean up the variable to be verified*/
    target_status[2].OPERATING_MODE = -1;

	/* obtain mode in partition status of target partition p3 */
    GET_A_PARTITION_STATUS(target_ids[2], &target_status[2], &rc);

    /* partition p3 should be in mode COLD_START */
    if(target_status[2].OPERATING_MODE != COLD_START){
    	test_error = 1;
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 43 
    	Set mode of target partitions to IDLE; trans: WARM_START->IDLE.
	trans: COLD_START->IDLE. trans: NORMAL->IDLE. */
    test_step_announce(43,1);

    /* Test step 43 code */
    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* set mode of target partitions to IDLE */
    	SET_A_PARTITION_MODE(target_ids[i], IDLE, &rc);

    	/* all operations should return NO_ERROR */
    	if(NO_ERROR != rc){
    		test_error = 1;
    	}
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
    	res &= test_step_report(    TEST_SUCCESS,
    			RESULT_EQUAL | RESULT_TYPE_VALUE,
    			ret);
    } else {
    	res &= test_step_report(    TEST_FAILURE,
    			RESULT_DIFF | RESULT_TYPE_VALUE,
    			ret);
    }

    rtems_task_wake_after(mtf_ticks);

    /* Test Step 44 
    	Verify that the target partitions mode is IDLE. */
    test_step_announce(44,1);

    /* Test step 44 code */
    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode IDLE */
        if(target_status[i].OPERATING_MODE != IDLE){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 45 
    	Set mode of target partitions to IDLE; invalid trans: IDLE->IDLE */
    test_step_announce(45,1);

    /* Test step 45 code */
    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* set mode of target partitions to IDLE */
    	SET_A_PARTITION_MODE(target_ids[i], IDLE, &rc);

    	/* all operations should return NO_ACTION */
    	if(NO_ACTION != rc){
    		test_error = 1;
    	}
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
    	res &= test_step_report(    TEST_SUCCESS,
    			RESULT_EQUAL | RESULT_TYPE_VALUE,
    			ret);
    } else {
    	res &= test_step_report(    TEST_FAILURE,
    			RESULT_DIFF | RESULT_TYPE_VALUE,
    			ret);
    }

    /* Test Step 46 
    	Verify that the target partitions mode is IDLE. */
    test_step_announce(46,1);

    /* Test step 46 code */
    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode IDLE */
        if(target_status[i].OPERATING_MODE != IDLE){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 47 
    	Set mode of target partitions to NORMAL; invalid trans: IDLE->NORMAL */
    test_step_announce(47,1);

    /* Test step 47 code */
    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* set mode of target partitions to NORMAL */
    	SET_A_PARTITION_MODE(target_ids[i], NORMAL, &rc);

    	/* all operations should return INVALID_MODE */
    	if(INVALID_MODE != rc){
    		test_error = 1;
    	}
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
    	res &= test_step_report(    TEST_SUCCESS,
    			RESULT_EQUAL | RESULT_TYPE_VALUE,
    			ret);
    } else {
    	res &= test_step_report(    TEST_FAILURE,
    			RESULT_DIFF | RESULT_TYPE_VALUE,
    			ret);
    }

    /* Test Step 48 
    	Verify that the target partitions mode is IDLE. */
    test_step_announce(48,1);

    /* Test step 48 code */
    rc			= -1;
    test_error 	= 0;

    for (i=0; i<3; i++){

    	/* first clean up the variable to be verified*/
    	target_status[i].OPERATING_MODE = -1;

    	/* obtain mode in partition status of target partitions */
        GET_A_PARTITION_STATUS(target_ids[i], &target_status[i], &rc);

        /* all partitions should be in mode IDLE */
        if(target_status[i].OPERATING_MODE != IDLE){
        	test_error = 1;
        }
    }

    /* EXPECTED: */
    if ((0 == test_error) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
    
    /* wait for prints */
    rtems_task_wake_after(mtf_ticks*2);

    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}


