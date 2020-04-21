
/* ***********************************************************************    */
/* ** TEST-DEF-80200 / 80200 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0190                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define a test with one supervisor partition, which has access to the I/O
	memory area. Check that the timer 0 is configured for the correct value (matching
	the number of ticks per second), and that the pmk_ISR_Clock_Handler is
	assigned to the Timer 0 interrupt. */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P0testdef.h>


/* Test external definitions **********************************************	*/
#include <pal_system.h>
#include <bsp.h>
#include <leon.h>
#include <sharedBSPs.h>
#include <amba.h>

/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 30;

/* Test auxiliary functions    ********************************************	*/
extern void pmk_ISR_Clock_Handler(void);

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
	int i = 0;
	unsigned int aux = 0;
	unsigned int clkirq = 0;
	unsigned int hndlr = 0;
	unsigned int *tbr = NULL;
	amba_apb_device dev;
	volatile LEON3_Timer_Regs_Map *regs = 0;
	
    /* Test Start ******************************************************    */
    test_enter(80200);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Obtain a pointer to the GPTIMER control registers */
    test_step_announce(0,1);

    /* Test step 0 code */
    i = amba_find_apbslv(&amba_conf, VENDOR_GAISLER, GAISLER_GPTIMER, &dev);
	if(i > 0){
		regs = (volatile LEON3_Timer_Regs_Map *) dev.start;
	}

    /* EXPECTED: */
    if ((0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 1 
    	Check that the reload value corresponds to the expected ms per tick. */
    test_step_announce(1,1);

    /* Test step 1 code */
    aux = regs->timer[0].reload;

    /* EXPECTED: */
    if ((aux == (pmk_get_usr_ms_per_tick() - 1)) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 2 
    	Check that the timer is enabled */
    test_step_announce(2,1);

    /* Test step 2 code */
    aux = regs->timer[0].conf;

    /* EXPECTED: */
    if (((aux & LEON3_GPTIMER_EN) == LEON3_GPTIMER_EN) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 3 
    	Check that the timer reload is automatic */
    test_step_announce(3,1);

    /* EXPECTED: */
    if (((aux & LEON3_GPTIMER_RL) == LEON3_GPTIMER_RL) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 4 
    	Check that the timer is set to activate the IRQ on each count end */
    test_step_announce(4,1);

    /* EXPECTED: */
    if (((aux & LEON3_GPTIMER_IRQEN) == LEON3_GPTIMER_IRQEN) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 5 
    	Check that entry GPTIMER interrupt entry of the trap table (obtained
	through the pmk_sparc_get_tbr system call) matches the address of the
	pmk_ISR_Clock_Handler function. */
    test_step_announce(5,1);

    /* Test step 5 code */
	
	/* obtain clock interrupt */
    clkirq = ((regs->status & 0xfc) >> 3) + 0x10;
	
	/* obtain trap base address */
	tbr = (unsigned int *) ((unsigned int)pmk_sparc_get_tbr() & (~0xFFF));
	
	/* parse the Trap Table entries */
	hndlr = ((tbr[(clkirq*4)+1] & 0x00FFFFFF) << 10) + (tbr[(clkirq*4)+2] & 0x00001FFF);
	
	aux = (uint32_t) &pmk_ISR_Clock_Handler;
	

    /* EXPECTED: */
    if ((aux == hndlr) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    
    /* Test End */
	rtems_task_wake_after(mtf_ticks);
    test_exit(res,mtf_ticks);
    return 0;
}


