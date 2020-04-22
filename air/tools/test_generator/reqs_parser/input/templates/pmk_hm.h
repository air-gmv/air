#ifndef _HM_H
#define _HM_H

#include <rtems.h>
#include <pmk_segregation.h>

/******************************************************************************
 * Definition of maximum number of errors and states of Health Monitor Table
 * TODO: modify the numbers to the values below once there is enough memory for
 * the partitions
 * #define PMK_MAX_ERR_NUM 21
 * #define PMK_MAX_STATE_NUM 7
 * 
 ******************************************************************************/

#define PMK_MAX_ERR_NUM 8


#define PMK_HM_SW_INTERRUPT_TRAP 134

#define PMK_HM_PRIORITY 3
#define PMK_PARTITION_PRIORITY 100

/****************************************************************************** 
 * PMK states assumed at PMK normal execution
 ******************************************************************************/

typedef enum {
	PMK_STATE_INIT 		= 1,
	PMK_STATE_SYS  		= 2,
	PMK_STATE_HM   		= 3,
	PMK_STATE_PAR 		= 4,
	PMK_MAX_STATE_NUM	= 5
} pmk_state_t;

/****************************************************************************** 
 * PMK states assumed for error handling - states used in the HM Table
 * Obs: Used within health monitor functions only.
 ******************************************************************************/
typedef enum {
	PMK_MOD_INIT 	= 1,
	PMK_MOD_SYS 	= 2,
	PMK_PAR_INIT 	= 4,
	PMK_PAR_PROC 	= 5,
	PMK_PAR_OS 		= 8,
	PMK_PAR_EH 		= 6,  /* PMK-HM assumes this state when an error is detected while already handling a previous error */
	PMK_MOD_HM 		= 9   /* Not used yet, but could be an ultimate state, leading to default actions to avoid system crash - not available to user */
} pmk_hm_state_id_t;


/****************************************************************************** 
 * PMK error codes for i386 exceptions
 ******************************************************************************/

typedef enum {
	PMK_CONFIG_ERR        = 1,  /* A653 */
	PMK_MOD_CONFIG_ERR    = 2,  /* A653 */
	PMK_PAR_CONFIG_ERR    = 3,  /* A653 */
	PMK_PAR_INIT_ERR      = 4,  /* A653 */
	PMK_SEG_ERR           = 5,  /* SEGMENTATION ERROR */
	PMK_TIME_ERR          = 6,  /* DEADLINE MISS */
	PMK_ILL_ERR           = 7,  /* ILLEGAL INSTRUCTION */
	PMK_NUM_ERR           = 8,  /* NUMERICAL ERROR */
	PMK_DIV_ZER           = 8,  /* DIVIDE BY ZERO */
	PMK_OVRFLW_ERR        = 9,  /* OVERFLOW */
	PMK_FLOAT_ERR         = 10, /* FLOATING POINT ERROR */
	PMK_APP_ERR           = 11, /* APPLICATION ERROR */
	PMK_STK_ERR           = 12, /* STACK ERROR */
	PMK_SYS_ERR           = 13, /* A653 PARITY ERROR - HW ERROR FOR APPLICATION LEVEL*/
	PMK_POW_ERR           = 14, /* POWER INTERRUPT */
	PMK_DBG_ERR           = 15, /* DEBUG ERROR */
	PMK_IO_ERR            = 16, /* IO ERROR */
	PMK_DOUB_ERR          = 17, /* DOUBLE FAULT */
	PMK_VIOL_ERR          = 18, /* VIOLATION ERROR */
	PMK_DEV_ERR           = 19, /* DEVICE ERROR */
	PMK_POS_ERR           = 20  /* APEX ERROR*/
        
} pmk_hm_err_id_t;


/****************************************************************************** 
 * PMK action code - Health Monitor Table
 ******************************************************************************/

typedef enum {
	IGNORE 		= 0,
	SHUTDOWN 	= 1,
	RESET 		= 2,
	SET_IDLE 	= 3,
	SET_WARM 	= 4,
	SET_COLD 	= 5
} pmk_hm_act_t;


/****************************************************************************** 
 * PMK error levels code - Health Monitor Table - A653 defined
 ******************************************************************************/

typedef enum {
	PROCESS 	= 0,
	PARTITION 	= 1,
	MODULE 		= 2
} pmk_hm_level_t;


/******************************************************************************
 * Data types used in the Health Monitor Table
 *****************************************************************************/

typedef enum
{
	PMK_DEADLINE_MISSED 	= 0,
	PMK_APPLICATION_ERROR 	= 1,
	PMK_NUMERIC_ERROR 		= 2,
	PMK_ILLEGAL_REQUEST 	= 3,
	PMK_STACK_OVERFLOW 		= 4,
	PMK_MEMORY_VIOLATION 	= 5,
	PMK_HARDWARE_FAULT 		= 6,
	PMK_POWER_FAIL 			= 7
} apx_proc_error_code_t;


typedef struct pmk_hm_err_act_st{
        int 			error_id; /*pmk_hm_part_err_id_t or pmk_hm_err_id_t*/
        pmk_hm_act_t 	action_id;
}pmk_hm_err_act_t;

typedef struct pmk_hm_err_lvl_st{
        int 			error_id; /*pmk_hm_part_err_id_t or pmk_hm_err_id_t*/
        pmk_hm_level_t 	level_id;
        apx_proc_error_code_t app_error_code;
}pmk_hm_err_lvl_t;

typedef struct pmk_hm_level_desc_st {
	pmk_hm_state_id_t 	state_id;
	pmk_hm_err_lvl_t  	lvl_err[PMK_MAX_ERR_NUM];
} pmk_hm_level_desc_t;

typedef struct pmk_hm_tab_st {
	pmk_hm_state_id_t 	state_id;
	pmk_hm_err_act_t 	error_action[PMK_MAX_ERR_NUM];
} pmk_hm_tab_t;



typedef struct pmk_hm_part_ctrl_st {
	
	pmk_hm_tab_t 		part_hmtab[PMK_MAX_STATE_NUM];
	/*
	PartitionHMCallback         partition_hm_callback;
	ModuleHMCallback            module_hm_callback;
	PartitionState              partition_state;
	PartitionEHStart            start_process_error_handler;
	PartitionRegisterError      register_error;
	PartitionHasEH              process_error_handler;
	PartitionStateError         partition_state_error;	
	*/
} pmk_hm_part_ctrl_t;

/* Main health monitor control structure (one per partition) */        
typedef struct pmk_hm_ctrl_st {		
	/* System control structures */	
	pmk_hm_level_desc_t systab[PMK_MAX_STATE_NUM]; 		//HM - at most 6 state entries for pmk
	
	/* Module control structures */
	pmk_hm_tab_t 		mod_hmtab[PMK_MAX_STATE_NUM];
	
	/* Partition Control Structures	*/
	pmk_hm_part_ctrl_t	part_ctrl[PMK_MAX_NUMBER_PARTITIONS];
} pmk_hm_ctrl_t;

/******************************************************************************
 * Private functions interface
 *****************************************************************************/ 
 /* Get health monitor control structures	*/
pmk_hm_ctrl_t * pmk_get_hm_ctrl (void);


/******************************************************************************
 * Public functions interface
 *****************************************************************************/ 

/* Initialization of health monitoring tables */
int pmk_init_part_hm_table(int partition_number);
int pmk_init_sysmod_hm_table(void);

rtems_task pmk_health_monitor();
void pmk_start_health_monitor(uint32_t trap);
void pmk_suspend_faulty(rtems_id faulty_tid);

#endif


