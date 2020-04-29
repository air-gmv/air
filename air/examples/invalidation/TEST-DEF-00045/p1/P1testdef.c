
/* ***********************************************************************    */
/* ** TEST-DEF-00041 / 41 *********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    RB-01110                                                                */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Confirm that the clock tick handler takes less than TBD_A microseconds to
	return the execution to the executing partition everytime a partition change is
	not needed. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>
#include "../testdef.h"

/* Test external definitions **********************************************	*/

/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= MTF_TICKS;
static char buf[NUMBER_OF_POINTS];

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
                                        
	int i,o,z,k,n,t01, t10;
	volatile int *p_t1 = (int *)(0x80000300 +  2 * sizeof(int) * 4);
	/* get_timer1_pointer returns a pointer to a shared area between partitions
	 * thus, it is meant to hold timer1 values, but filling those values / 
	 * entries is the responsability of the test application.
	 * Thus, the very simple 'structure' defined next */
	int x1 = 0;
	int x2 = 0;
	int pivo = 0;
										
    /* Test Steps *******************************************************    */
    /* Test Step 1 
    	Start (almost) infinite cycle of timer2 count prints, to
		mark with an accurate timestap (1us) each instant passed in 
		partition 1. 
		Please note that the C: :D markers are needed to properly identify
		uninterrupted printf operations (only those can be considered
		reliable). */

    /* Test step 1 code */
    test_step_announce(1,1);
    pprintf("\n");
	rtems_task_wake_after(mtf_ticks >> 1);
	

	/* Important note: timer2 is decreasing, so the logic must be reversed */
	for (k=0;k<NUMBER_CYCLES;k++) {
		x1 = *p_t1;
		pivo = x1;
		n	= 0;
		while (pivo - x1 <= (NUMBER_OF_POINTS-1)) {
			x2 = *p_t1;
			if (x1 - x2 >= 1) {
				i = pivo - x2;
				if (i >= 0 && i < NUMBER_OF_POINTS) buf[i] = 1;
				x1 = *p_t1;			
			} else {
				n += 1;
			}
		}
		o = 0;
		z = 0;
		t10 = 0;
		t01 = 0;
		for (i=0;i<(NUMBER_OF_POINTS );i++) {		
			if (buf[i] == 1)	o += 1;
			else				z += 1;
			/* Transitions 0->1*/
			if ((i>0) && (buf[i] == 0) && (buf[i-1] == 1)) {
				t01 += 1;
//				pprintf("[1] 1->0 i:%i\n",i);
			}
			/* Transitions 1->0*/
			if ((i>0) && (buf[i] == 1) && (buf[i-1] == 0)) {
				t10 += 1;
//				pprintf("[1] 0->1 i:%i\n",i);
			}
		}
		for (i=0;i<NUMBER_OF_POINTS;i++) buf[i] = 0;
	//	printf("P[1] 1:(%06i)|0:(%06i)|n:(%06i)|t:(%06i)\n",o,z,n,t10);

		rtems_task_wake_after(mtf_ticks >> 1);
	}

    
	/* Test End */
    test_return();
    return 0;
}

