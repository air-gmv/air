/* ===========================================================================
 * This file contains information protected by Portuguese and international copyright law
 * ===========================================================================
 * (C) Skysoft Portugal SA, 2008-2010
 * ===========================================================================
 * Program  : PMK - Partition Management Kernel
 * Component: usr_configuration
 * Purpose  : User partition scheduling configuration
 * Status   : Stable
 * Level    : N/A
 * Author   : Joao Cristovao
 * ===========================================================================
 * Revision Control
 * ===========================================================================
 * $Source: /IMADE/samples/samples/control/config/usr_schedules.h,v $
 * $Branch:  $
 * $Revision: 1.2 $
 * $CommitId: d304dfb75eb4347 $
 * $Author: jaac $
 * $Date: 2011/06/17 15:42:36 $
 * ===========================================================================*/

#include <rtems.h>
 
/* System microseconds per tick (shared between all partitions) */
#define PMK_MICROSECONDS_PER_TICK RTEMS_MILLISECONDS_TO_MICROSECONDS(${ms_per_tick})
 
/* Number of different partition schedules		*/
#define PMK_NUM_OF_SCHEDULES 		(${num_sched})

/* Number of preemption points for each schedule 	*/
/* As many as PMK_NUM_OF_SCHEDULES. */
% for s_num in range(0,num_sched):
#define PMK_NUM_OF_PREEMPT_POINTS_SCHED_${s_num}  (${sch_preempt_points[s_num]})
% endfor


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

