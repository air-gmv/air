
/* ***********************************************************************    */
/* ** TEST-DEF-80080 / 80080 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0080                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Create a test with two partitions, with similar code, each with at least
	one task. Corrupt the value of both _Thread_Executing and _Thread_Heir
	variables in the first partition, and check that only the first partition raises
	Health Monitor events. The second partition should continue its execution without
	changes. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P0testdef.h>
#include <md5.h>


/* Test external definitions **********************************************	*/
#define SECTIONS (1)

/* Test type definitions **************************************************	*/
typedef md5_byte_t md5_t[16];
typedef md5_t md5_pair[2];
typedef struct {
	char *p;
	int size;
} section_t;

typedef enum task_states  {
	TASK_ERROR 		= -1,
	TASK_UNINIT		= 0,
	TASK_STARTED	= 1,
	TASK_GOOD_CYCLE	= 2
} task_states_t;


/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 30;
int task1_status 	= TASK_UNINIT;
int task2_status	= TASK_UNINIT;

/* Test auxiliary functions    ********************************************	*/
void fill_section(section_t* section, void* start, void* end) {
	section->p = start;
	section->size = (((int) ((char*)end - (char*)start)) + 4095) & ~4095;
}


//* Test auxiliary functions    ********************************************	*/
rtems_task Thread1(rtems_task_argument argument) {
	
	int j		= 0;
	section_t sections	[SECTIONS];
	md5_pair  md5s		[SECTIONS];
	md5_state_t state;

	/* Test specific variables  ******************************************	*/

	/* Initalisation code */
	fill_section(&(sections[6]),&air_memmap_pos_code_start		, &air_memmap_pos_code_end	);
	for (j=0;j < SECTIONS; j += 1) {
		md5_init	(&state);
		md5_append	(&state, (const md5_byte_t *)sections[j].p, sections[j].size);
		md5_finish	(&state,md5s[j][0]);
	}

	task1_status = TASK_STARTED;
	rtems_task_wake_after(mtf_ticks >> 1);

	/* Main cycle */
    while(1) {
		/* If we ever reach here, it means partition idle mode is not working,
		 * or _Thread_Heir and/or _Thread_Executing were not altered */
    	test_exit(TEST_FAILURE,mtf_ticks);
		rtems_task_wake_after(1);
    }
}

rtems_task Thread2(rtems_task_argument argument) {
	
	int j		= 0;
	section_t sections	[SECTIONS];
	md5_pair  md5s		[SECTIONS];
	md5_state_t state;

	/* Test specific variables  ******************************************	*/

	/* Initalisation code */
	fill_section(&(sections[6]),&air_memmap_pal_code_start		, &air_memmap_pal_code_end	);
	for (j=0;j < SECTIONS; j += 1) {
		md5_init	(&state);
		md5_append	(&state, (const md5_byte_t *)sections[j].p, sections[j].size);
		md5_finish	(&state,md5s[j][0]);
	}
	
	task2_status = TASK_STARTED;
	rtems_task_wake_after(mtf_ticks >> 1);

    while(1) {
		/* If we ever reach here, it means partition idle mode is not working,
		 * or _Thread_Heir and/or _Thread_Executing were not altered */
    	test_exit(TEST_FAILURE,mtf_ticks);
		rtems_task_wake_after(1);
    }
}


/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {


	/* function to test return code     */
    rtems_status_code ret    = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res     = TEST_SUCCESS;     

	/* signal error ocurrence	*/
    unexp_error	=  i_error;

    /* Test Step 10 
    	Check that the health monitor partition callback was called with the
		error PMK_SEG_ERR and put partition in idle mode. */
    test_step_announce(10,1);

    /* Test step 10 code */
	if (PMK_SEG_ERR == unexp_error) {
		ret = RTEMS_SUCCESSFUL;
	} else {
		ret = RTEMS_INTERNAL_ERROR;
	}

    /* EXPECTED: */
    if (RTEMS_SUCCESSFUL == ret) {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

	/* there is no valid task to return, since RTEMS is corrupted */
	test_return(res);
    return;
}

/* Test execution *******************************************************    */
int test_main (void) {
    /* Test generic variables  ******************************************	*/
	/* function to test return code     */
    rtems_status_code ret    = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res     = TEST_SUCCESS;     
    
    /* Test specific variables  ******************************************	*/
	
	/* Thread IDs */
	rtems_id thread1ID;
	rtems_id thread2ID;

    /* Test Start ******************************************************    */
    test_enter(80080);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Create two tasks. */
    test_step_announce(0,1);

    /* Test step 0 code */
	ret |= rtems_task_create(rtems_build_name(	'T' , 'H' , 'R' , '1') ,
		                               	30 ,
					                   	RTEMS_MINIMUM_STACK_SIZE ,
									   	RTEMS_PREEMPT ,
										RTEMS_NO_FLOATING_POINT ,
										&thread1ID);
	ret |=  rtems_task_create(rtems_build_name(	'T' , 'H' , 'R' , '2') ,
		                              	31 ,
					                   	RTEMS_MINIMUM_STACK_SIZE ,
									   	RTEMS_PREEMPT ,
										RTEMS_NO_FLOATING_POINT ,
										&thread2ID);

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

    /* Test Step 1 
    	Start the first task, assigning it to a function that calculates 
		the MD5 of the POS code section, and then sleeps. At each 
		re-activation, the function should re-calculate the MD5 and 
		compare it to the value of the previous iteration (and then
		sleep again). */
    test_step_announce(1,1);

    /* Test step 1 code */
	ret |= rtems_task_start(thread1ID , Thread1 , 0);
	ret |= rtems_task_wake_after(1);
 
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

    /* Test Step 2 
    	Start the first task, assigning it to a function that calculates 
		the MD5 of the PAL code section, and then sleeps. At each 
		re-activation, the function should re-calculate the MD5 and 
		compare it to the value of the previous iteration (and then
		sleep again). */
    test_step_announce(2,1);

    /* Test step 2 code */
	ret |= rtems_task_start(thread2ID , Thread2 , 0);
	ret |= rtems_task_wake_after(1);
 
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

	/* Test Step 3
	   Check that both tasks have run, and then sleep to repeate the process in
	   the other partition */
	test_step_announce(3,1);

	/* Test step 3 code */
	if (task1_status == TASK_STARTED && task2_status == TASK_STARTED) {
		ret = RTEMS_SUCCESSFUL;
	} else {
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
	rtems_task_wake_after(mtf_ticks >> 1);

    /* Test Step 8 
    	Overwrite the value of RTEMS variables _Thread_Executing and
	_Thread_Heir to an invalid value (-1). */
    test_step_announce(8,1);

    /* Test step 8 code */
    _Thread_Executing 	= (Thread_Control *) -1;
	_Thread_Heir		= (Thread_Control *) -1;

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

    /* Test Step 9 
    	Suspend main thread (with rtems_task_wake_after (1) ). */
    test_step_announce(9,1);

    /* Test step 9 code */

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
	ret |= rtems_task_wake_after(1);

    /* Test End */
	/* It should never reach here, it means that the partition was not put into idle 
	 * mode by our custom health monitoring partition callback */
    test_exit(TEST_FAILURE,mtf_ticks);
    return 0;
}


