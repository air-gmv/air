
/* ***********************************************************************    */
/* ** TEST-DEF-00041 / 41 *********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    RB-01110                                                                */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	
 * Purpose:
 * --------
 * The program validates the time partitioning capabilities
 * of the operating system and can also be used
 * to bench mark the partition switch.

 * How it works
 * ------------
 * The program writes a buffer of 100000 bytes.
 * The buffer segments are initialised to 0.
 * The buffer represents micro second slots
 * in a 100ms time span.
 *
 * The process running in the program performs two tasks:
 * First it tries to fill all 100000 slots of the buffer,
 * i.e. it runs until 100ms are elapsed, writing one
 * buffer segment each time a us has passed.
 * Then it reports the buffer back "home", i.e.
 * it writes to stdout, a file or a queue.
 *
 * If the program runs without time partitiong,
 * all but one slot shall be written. 
 * With time partitioning 
 * (and with the above mentioned timing constraints)
 * only 25.000 slots shall be written, 
 * i.e. the slots of the first time window.
 * With a granularity of 1us,
 * a very clear picture of the partitioning switch
 * overhead of the OS results,
 * which is written slots - 25.000.
 * During evaluation, care must be taken
 * where the additional slots are written.
 * It might occasionally happen 
 * that the process writes some slots in the second window
 * of this application.
 */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P0testdef.h>
#include "../testdef.h"

/* Test external definitions **********************************************	*/

/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= MTF_TICKS;
static char buf[NUMBER_OF_POINTS];


/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void hm_part_callback (pmk_hm_err_id_e i_error,void *i_state) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    //pmk_hm_state_id_e state = (pmk_hm_state_id_e) i_state;
	/* signal error ocurrence	*/
    unexp_error	= 1;
    return;
}

/* Test execution *******************************************************    */
int test_main (void) {
    /* Test generic variables  ******************************************	*/
	/* total test result                */
    int res     = TEST_SUCCESS;     
    
    /* Test specific variables  ******************************************	*/
	int i,o,z,k,n,t10,t01;
	volatile int *p_t1 = (int *)(0x80000300 +  2 * sizeof(int) * 4);
	/* get_timer1_pointer returns a pointer to a shared area between partitions
	 * thus, it is meant to hold timer1 values, but filling those values / 
	 * entries is the responsability of the test application.
	 * Thus, the very simple 'structure' defined next */
	int x1 = 0;
	int x2 = 0;
	int pivo = 0;
    /* Test Start ******************************************************    */
    test_enter_postprocssd(45);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Start (almost) infinite cycle of timer2 count prints, to
		mark with an accurate timestap (1us) each instant passed in 
		partition 0 
		Please note that the A: :B markers are needed to properly identify
		uninterrupted printf operations (only those can be considered
		reliable). */
		
    test_step_announce(0,1);
	pprintf("\n");
	rtems_task_wake_after(mtf_ticks >> 1);
    
	/* Test step 0 code */
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
				//pprintf("[0] 1->0 i:%i\n",i);
			}
			/* Transitions 1->0*/
			if ((i>0) && (buf[i] == 1) && (buf[i-1] == 0)) {
				t10 += 1;
				//pprintf("[0] 0->1 i:%i\n",i);
			}
		}
		for (i=0;i<NUMBER_OF_POINTS;i++) buf[i] = 0;
	//	printf("P[0] 1:(%06i)|0:(%06i)|n:(%06i)|t:(%06i)\n",o,z,n,t10);
		rtems_task_wake_after(mtf_ticks >> 1);

	}
	rtems_task_wake_after(mtf_ticks );
	//pprintf("\n");

    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}

