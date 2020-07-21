/* ===========================================================================
 * This file contains information protected by Portuguese and international copyright law
 * ===========================================================================
 * (C) Skysoft Portugal SA, 2008-2010
 * ===========================================================================
 * Program  : PAL - Partition Adaptation Layer
 * Component: pal_callbacks
 * Purpose  : Partition callbacks table
 * Status   : Stable
 * Level    : N/A
 * Author   : Joao Cristovao
 * ===========================================================================
 * Revision Control
 * ===========================================================================
 * $Source: /IMADE/PMK2/src/pal/pal_callbacks.c,v $
 * $Branch:  $
 * $Revision: 1.4 $
 * $CommitId: d3c4e673e1b3bc1 $
 * $Author: jaac $
 * $Date: 2011/09/07 09:49:16 $
 * ===========================================================================*/


#include <rtems.h>
#include <pal_callbacks.h>
#include <pmk_segregation.h>

/* Type definitions ****************************************************	*/
<% part_num = len(part_nums) %>
/** Array of callback functions to be defined at each partition */
typedef pal_function_pointer pal_callbacks_table_t 	[${part_num}]
													[PMK_CALLBACK_COUNT];
												   

/* Partition Callback Function Headers **********************************	*/

/* The logic for the weak attribute would be have a set of fully defined 	*
 * functions, that if not defined would default to zero. But this way the 	*
 * callback initialization definition could even be statically defined.		*/

/* Per partition (number) definitions *************************************	*/

% for p in part_nums:
/* PMK_CALLBACK_AIRPEX_ENTRY	*/	
rtems_status_code P${p}entry_point 				(int i,void *p) __attribute__((weak));
/* PMK_CALLBACK_AIRPEX_TICK	*/	
rtems_status_code P${p}apx_watchdog_callback 	(int i,void *p) __attribute__((weak));
/* PMK_CALLBACK_GET_PORTS	*/	
rtems_status_code P${p}get_usr_ports 			(int i,void *p) __attribute__((weak));
/* PMK_CALLBACK_REFRESH_PORTS	*/	
rtems_status_code P${p}refresh_ports 			(int i,void *p) __attribute__((weak));
/* PMK_CALLBACK_KTHREAD_SET	*/	
rtems_status_code P${p}set_kthread_id 			(int i,void *p) __attribute__((weak));
/* PMK_CALLBACK_RESUME_KTHREAD	*/	
rtems_status_code P${p}resume_kthread 			(int i,void *p) __attribute__((weak));
/* PMK_CALLBACK_AIRPEX_GET_FAULTY	*/	
rtems_status_code P${p}apx_suspend_curr_faulty 	(int i,void *p) __attribute__((weak));
/* PMK_CALLBACK_AIRPEX_GET_PSTATE	*/	
rtems_status_code P${p}apx_get_partition_state_error	(int *o_state, int *o_error) __attribute__((weak));
/* PMK_CALLBACK_HM_PART_CALLBACK	*/	
rtems_status_code P${p}hm_part_callback		(int error, int state) __attribute__((weak));
/* PMK_CALLBACK_AIRPEX_DETECT_EH */
rtems_status_code P${p}apx_is_process_eh_defined(void) __attribute__((weak));
/* PMK_CALLBACK_AIRPEX_REG_ERROR */
int P${p}apx_register_error 					(int ERROR_CODE, char * MESSAGE_ADDR, int LENGTH, int tid, unsigned long p_eip) __attribute__((weak));
/* PMK_CALLBACK_AIRPEX_ACTIV_EH */
int P${p}apx_start_error_handler_process 		(void) __attribute__((weak));
/* PMK_CALLBACK_GET_CONFIGURATION */
int P${p}get_configuration 						(void) __attribute__((weak)) ;
/* PMK_CALLBACK_INIT_USER_TASKS_P */
int P${p}get_initialize_user_tasks_p				(void) __attribute__((weak));
% endfor

/* Per partition type definitions ****************************************	*/
/* PMK_CALLBACK_POS_ENTRY 			*/
	/* non-weak - these symbols are mandatory */
extern void boot_card (void);
extern void __wrap_boot_card(void);

/* PMK_CALLBACK_INCR_ISR_NEST 		*/
	/* non-weak - these symbols are mandatory */
void pal_rtems_incr_nest_level 			(void);
void pal_no_os_incr_nest_level			(void);

/* PMK_CALLBACK_THREAD_DISPATCH 	*/
	/* non-weak - these symbols are mandatory */
void _Thread_Dispatch					(void);
void pal_no_os_Thread_Dispatch			(void);

/* PMK_CALLBACK_POS_CLOCK_TICK 		*/
rtems_status_code rtems_clock_tick		(void);
rtems_status_code rtems_clock_update(long ticks);

/*PMK_CALLBACK_POS_UPDATE_TOD*/
void _TOD_Update(const struct timespec *time);

/* PMK_CALLBACK_POS_ISR_DISPATCHER 	*/
	/* non-weak - these symbols are mandatory */
extern void pal_rtems_isr_dispatcher	(void);
extern void pal_no_os_isr_dispatcher	(void);

/* PMK_CALLBACK_POS_IS_SYS_READY 	*/
	/* non-weak - these symbols are mandatory */
extern void pal_is_system_ready			(void);


/* Efective callback table ***********************************************	*/
pal_callbacks_table_t pal_callbacks_table = {
% for p in part_nums:
{	/* partition ${p} **********************************************	*/
	/* PMK_CALLBACK_INVALID	*/
	NULL,							
	/* PMK_CALLBACK_POS_ENTRY			*/
	% if 	partitions[p]["type"] == "TEST_EDI_UNT":
	(pal_function_pointer) __wrap_boot_card,
	% else:
	(pal_function_pointer) boot_card,
	% endif
	/* PMK_CALLBACK_AIRPEX_ENTRY	*/
	P${p}entry_point,
	/* PMK_CALLBACK_AIRPEX_TICK	*/
	% if 	partitions[p]["apex"] == "AIRPEX":
	P${p}apx_watchdog_callback,
	% else:
	NULL,
	%endif
	/* PMK_CALLBACK_GET_PORTS	*/
	P${p}get_usr_ports,
	/* PMK_CALLBACK_REFRESH_PORTS	*/
	P${p}refresh_ports,
	/* PMK_CALLBACK_KTHREAD_SET	*/
	P${p}set_kthread_id,
	/* PMK_CALLBACK_RESUME_KTHREAD	*/
	P${p}resume_kthread,
	/* PMK_CALLBACK_INCR_ISR_NEST	*/
	% if 	partitions[p]["type"] == "TEST_EDI_UNT":
	(pal_function_pointer) pal_no_os_incr_nest_level, 	
	% else:
	(pal_function_pointer) pal_rtems_incr_nest_level,
	% endif
	/* PMK_CALLBACK_THREAD_DISPATCH	*/
	% if 	partitions[p]["type"] == "TEST_EDI_UNT":
	(pal_function_pointer)pal_no_os_Thread_Dispatch,
	% else:
	(pal_function_pointer)_Thread_Dispatch,
	% endif
	/* PMK_CALLBACK_POS_CLOCK_TICK	*/
	(pal_function_pointer) rtems_clock_update,
	/* PMK_CALLBACK_POS_ISR_DISPATCHER	*/
	% if 	partitions[p]["type"] == "TEST_EDI_UNT":
	(pal_function_pointer) pal_no_os_isr_dispatcher,
	% else:
	(pal_function_pointer) pal_rtems_isr_dispatcher,
	% endif
	/* PMK_CALLBACK_POS_IS_SYS_READY	*/
	% if 	partitions[p]["os"] == "NONE":				
	NULL,
	% else:
	(pal_function_pointer) pal_is_system_ready,
	% endif
	/* PMK_CALLBACK_AIRPEX_GET_PSTATE	*/
	% if 	partitions[p]["apex"] == "AIRPEX":
	(pal_function_pointer) P${p}apx_get_partition_state_error,
	% else:
	NULL,
	% endif
	/* PMK_CALLBACK_HM_PART_CALLBACK	*/
	(pal_function_pointer) P${p}hm_part_callback,
	/* PMK_CALLBACK_AIRPEX_DETECT_EH */
	% if 	partitions[p]["apex"] == "AIRPEX":
	(pal_function_pointer) P${p}apx_is_process_eh_defined,
	% else:
	NULL,
	% endif
	/* PMK_CALLBACK_AIRPEX_REG_ERROR */
	% if 	partitions[p]["apex"] == "AIRPEX":
	(pal_function_pointer) P${p}apx_register_error,
	% else:
	NULL,
	% endif
	/* PMK_CALLBACK_AIRPEX_ACTIV_EH */
	% if 	partitions[p]["apex"] == "AIRPEX":
	(pal_function_pointer) P${p}apx_start_error_handler_process,
	% else:
	NULL,
	% endif
	/* PMK_CALLBACK_GET_CONFIGURATION */
	(pal_function_pointer) P${p}get_configuration,
	/*PMK_CALLBACK_POS_UPDATE_TOD*/
	_TOD_Update,
	/* PMK_CALLBACK_INIT_USER_TASKS_P */
	(pal_function_pointer) P${p}get_initialize_user_tasks_p,
},
% endfor
};

/* ************************************************************************* */
/** Returns a (partition) function callback of the specified type
 *  \param [in] i_partition	: partition whose callback is needed
 *  \param [in] i_callback	: callback type
 *  \return callback function pointer
 */
 /* ************************************************************************* */
pal_function_pointer pal_get_callback	(int i_partition, 
										 pal_callbacks_e i_callback) {
	/* input validation */
	if ((i_partition < 0) || (i_partition >= PMK_MAX_NUMBER_PARTITIONS) ||
		(i_callback < PMK_CALLBACK_MIN) || (i_callback > PMK_CALLBACK_MAX)) {
		/* error: return NULL pointer	*/
		return NULL;
	} else {
		return pal_callbacks_table[i_partition][i_callback];
	}
}
	
/* ************************************************************************* */
/** Returns a partition callback table pointer
 *  \param [in] i_partition	: partition whose callback is needed
 *  \return callback table address
 */
 /* ************************************************************************* */
pal_function_pointer pal_get_callback_table	(int i_partition) {
	/* input validation */
	if ((i_partition < 0) || (i_partition >= PMK_MAX_NUMBER_PARTITIONS)) {
		/* error: return NULL pointer	*/
		return NULL;
	} else {
		return pal_callbacks_table[i_partition];
	}
}
	
/* ===========================================================================
 * This file contains information protected by Portuguese and international copyright law
 * ===========================================================================
 * (C) GMVIS Skysoft S.A., 2008-2010
* ============================================================================
* This file is part of GMV's Partition Management Kernel (PMK).
* The PMK is free software: 
 * you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The PMK is distributed in the hope 
 * that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the PMK.  
 * If not, see <http://www.gnu.org/licenses/>.
 * ===========================================================================
 */

