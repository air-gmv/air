
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

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>


/* Test external definitions **********************************************	*/
#include <pal_system.h>
#include <bsp.h>
#include <leon.h>
#include <sharedBSPs.h>
#include <amba.h>

/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int err_id = 0;
int mtf_ticks    	= 30;

/* Test auxiliary functions    ********************************************	*/
extern void pmk_ISR_Clock_Handler(void);

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    
	/* signal error ocurrence	*/
    unexp_error	= 1;
	err_id = i_error;
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
	rtems_isr_entry *old;
	volatile LEON3_Timer_Regs_Map *regs = 0;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 6 
    	Check that entry GPTIMER interrupt entry of the trap table (obtained
	through the pmk_sparc_get_tbr system call) matches the address of the
	pmk_ISR_Clock_Handler function. */
    test_step_announce(6,1);
	
	i = amba_find_apbslv(&amba_conf, VENDOR_GAISLER, GAISLER_GPTIMER, &dev);
	if(i > 0){
		regs = (volatile LEON3_Timer_Regs_Map *) dev.start;
	}
	
    /* obtain clock interrupt */
    clkirq = ((regs->status & 0xfc) >> 3) + 0x10;
	
	/* obtain trap base address */
	tbr = (unsigned int *) ((unsigned int)pmk_sparc_get_tbr() & (~0xFFF));
	
	/* parse the Trap Table entries */
	hndlr = ((tbr[(clkirq*4)+1] & 0x00FFFFFF) << 10) + (tbr[(clkirq*4)+2] & 0x00001FFF);
	
	aux = (uint32_t) &pmk_ISR_Clock_Handler;

    /* EXPECTED: */
    if ((aux == hndlr) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 7 
    	Register a new handler with rtems_interrupt_catch. */
    test_step_announce(7,1);

    /* Test step 7 code */
    rtems_interrupt_catch((rtems_isr_entry) test_main, clkirq, old);

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (1 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 8 
    	Check that the health monitor partition callback was called with the
	error PMK_VIOL_ERR. */
    test_step_announce(8,1);

    /* EXPECTED: */
    if (err_id == PMK_VIOL_ERR)  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    
    /* Test End */
    test_return();
    return 0;
}


