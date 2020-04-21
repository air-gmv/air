
/* ***********************************************************************    */
/* ** TEST-DEF-80210 / 80210 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0210                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define a system with two partitions with the following data: P1:
	ExWindow=10ms, Period=20ms, LocalTime P2: ExWindow=10ms, Period=20ms, GlobalTime
	TicksPerSecond=2ms Define a program that runs in both partitions with the following
	pseudo-code running in cycle: GET_TIME rtems_task_wake_after(1) Check that the values
	returned by GET_TIME are aproximatelly constant for the first partition after
	running the cycle for 20 times, and that for the second partition the value
	reported is 5 times the regular value each 5 cycles. */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P1testdef.h>

#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 15;
unsigned int nanoseconds_per_tick = 5000000;

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
	RETURN_CODE_TYPE RC;
	SYSTEM_TIME_TYPE OLD_TIME;
	SYSTEM_TIME_TYPE NEW_TIME;
	
	int ticks = 0;
    
	/* wait for next mtf */
	rtems_task_wake_after(10);	
	
    /* Test Steps *******************************************************    */
    /* Test Step 3 
    	Obtain on-board time using GET_TIME. Verify return code. Wait for next mtf */
    test_step_announce(3,1);

    GET_TIME(&NEW_TIME, &RC);

    /* EXPECTED: */
    if ((NO_ERROR == RC) &&  (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	
	for(repeat = 0; repeat < 20; repeat++){	
		/* Test Step 4 
			Sleep for 1 tick. */
		test_step_announce(4,repeat);
		
		OLD_TIME = NEW_TIME;
		
		/* EXPECTED: */
		if(0 == unexp_error){
			res &= test_step_report(    TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_step_report(    TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}
		
		rtems_task_wake_after(1);
		ticks++;

		/* Test Step 5 
			Obtain on-board time using GET_TIME. Verify that the time difference
		with the previously obtained time is approximately equal to the
		miliseconds per tick value except every 5 cycles in which this value should be
		approximately equal to the partition's period. */
		test_step_announce(5,repeat);

		GET_TIME(&NEW_TIME, &RC);
		
		/* every five cycles there is a partition context switch */
		if(ticks == 5){
			
			/* period is 25 ticks, therefore time will be increased by nanoseconds_per_tick*25*/
			OLD_TIME += nanoseconds_per_tick*25;
			
			/* reset cycle counter */
			ticks = 0;
		}

		/* EXPECTED: */
		if ((NO_ERROR == RC) && (0 == unexp_error) && 
		   ((NEW_TIME - OLD_TIME) == nanoseconds_per_tick) ){
		   
			res &= test_step_report(    TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_step_report(    TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}

    }
	
    /* Test End */
    test_return();
    return 0;
}


