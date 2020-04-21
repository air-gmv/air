/* ===========================================================================
 * This file contains information protected by Portuguese and international 
 * copyright law
 * ===========================================================================
 * (C) Skysoft Portugal SA, 2008-2010
 * ===========================================================================
 * Program  : PMK - Partition Management Kernel
 * Component: usr_configuration
 * Purpose  : N/A
 * Status   : Stable
 * Level    : N/A
 * Author   : PMK_TOOL_GEN
 * ===========================================================================
 * Revision Control
 * ===========================================================================
 * $Source: /IMADE/samples/samples/control/config/usr_schedules.c,v $
 * $Branch:  $
 * $Revision: 1.3 $
 * $CommitId: a044e2ffe5b5037 $
 * $Author: jaac $
 * $Date: 2011/07/27 12:02:35 $
 * ===========================================================================*/

#include <usr_schedules.h>
#include <pmk_schedule.h>

/* Number of different schedules (specified in usr_schedules.h) */
static pmk_module_schedule_t  pmk_usr_schedules[PMK_NUM_OF_SCHEDULES];  

/* Number of execution window per schedule     	*/
/* One of these tables is needed per schedule!  */
% for s_num in range(0,num_sched):
static pmk_preemption_point_t pmk_usr_points_${s_num}[PMK_NUM_OF_PREEMPT_POINTS_SCHED_${s_num}]; 
% endfor


/* ************************************************************************* */
/** Returns the number of user defined partition schedules
 *  \fn pmk_init_usr_schedules
 *  \return pmk_usr_schedules pointer
 *  \note This is the size of the pmk_usr_schedules table, PMK_NUM_OF_SCHEDULES
 */
 /* ************************************************************************* */
/* Initializes the user schedules using (also) the received partition control structure pointers */
void pmk_init_usr_schedules(pmk_partition_t * partition_table) {
  
% for s_num in range(0,num_sched):  
	/* Setup of schedule ${s_num} ********************************  */
	
	/* Number of execution windows in this schedule  */
	pmk_usr_schedules[${s_num}].number_preempt_points   = PMK_NUM_OF_PREEMPT_POINTS_SCHED_${s_num}; 
	/* Equal to "InitialModuleSchedule" in Config   */
	% if s_num == init_sched:
	pmk_usr_schedules[${s_num}].is_initial_schedule   = 1;
	% else:
	pmk_usr_schedules[${s_num}].is_initial_schedule   = 0;
	% endif

	/* first schedule preemption point table      */
	/* empty as of yet, will be filled next       */
	pmk_usr_schedules[${s_num}].preempt_point_table = pmk_usr_points_${s_num};

	% for preempt_pt in range(0,sch_preempt_points[s_num]):
	/* WindowStart in ticks */
	pmk_usr_schedules[${s_num}].preempt_point_table[${preempt_pt}].tick      = ${sch_preempt_ticks[s_num][preempt_pt][0]};
	/* The partition this Window is allocated to */
	pmk_usr_schedules[${s_num}].preempt_point_table[${preempt_pt}].partition = &partition_table[${sch_preempt_ticks[s_num][preempt_pt][1]}];
	% endfor
	
	/* MajorFrameSeconds */
	pmk_usr_schedules[${s_num}].major_time_frame = ${sch_mtfs[s_num]};
% endfor
}

/* ************************************************************************* */
/** Returns a pointer to the user defined partition schedules table
 *  \fn pmk_get_usr_schedules
 *  \return pmk_usr_schedules pointer
 *  \note   
 */
 /* ************************************************************************* */
pmk_module_schedule_t * pmk_get_usr_schedules() {
	return pmk_usr_schedules;
}

/* ************************************************************************* */
/** Returns the number of user defined partition schedules
 *  \fn pmk_get_number_schedules
 *  \return pmk_usr_schedules pointer
 *  \note This is the size of the pmk_usr_schedules table, PMK_NUM_OF_SCHEDULES
 */
 /* ************************************************************************* */
int pmk_get_usr_schedules_number() {
	return PMK_NUM_OF_SCHEDULES;
}

/* ************************************************************************* */
/** Returns the number of user defined microseconds (us) per tick
 *  \fn pmk_get_usr_ms_per_tick
 *  \return number of user defined microseconds (us) per tick
 */
 /* ************************************************************************* */
uint32_t pmk_get_usr_ms_per_tick(void) {
	return PMK_MICROSECONDS_PER_TICK;
}

/* ===========================================================================
 * This file contains information protected by Portuguese and international 
 * copyright law
 * ===========================================================================
 * (C) GMVIS Skysoft S.A., 2008-2010
 * ===========================================================================
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

