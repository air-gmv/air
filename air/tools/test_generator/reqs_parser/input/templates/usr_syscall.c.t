/* ===========================================================================
 * This file contains information protected by Portuguese and international copyright law
 * ===========================================================================
 * (C) Skysoft Portugal SA, 2008-2010
 * ===========================================================================
 * Program  : PMK - Partition Management Kernel
 * Component: usr_configuration
 * Purpose  : User partition configuration and 'getter' functions
 * Status   : Stable
 * Level    : N/A
 * Author   : Joao Cristovao
 * ===========================================================================
 * Revision Control
 * ===========================================================================
 * $Source: /IMADE/samples/samples/simple/config/usr_syscall.c,v $
 * $Branch:  $
 * $Revision: 1.3 $
 * $CommitId: 41c4e676f995d5f $
 * $Author: jaac $
 * $Date: 2011/09/07 13:20:26 $
 * ===========================================================================*/

#include "usr_partitions.h"
#include <pmk_segregation.h>
#include "usr_syscall.h"

/* 	[0] : PMK_SYSCALL_PERM_CONTEXT_HANDL 	  *********************************
			CPU_CONTEXT_SWITCH	0x-------1
			CPU_CONTEXT_RESTORE 0x-------2
			FP_CONTEXT_SAVE 	0x-------4
			FP_CONTEXT_RESTORE	0x-------8
	
	[1] : PMK_SYSCALL_PERM_INTERRUPT_HANDL    *********************************
			SYS_EXIT			0x-------1
			IRQ_DISABLE 		0x-------2
			IRQ_ENABLE		 	0x-------4
			GET_INTERR_LEVEL	0x-------8
			DPP_FLAG_RESET		0x------1-		delayed_plugin_processing
			FLOAT_POINT_ENABLE	0x------2-
			FLOAT_POINT_DISABLE 0x------4-
			CACHE_ENABLE		0x------8-
			CACHE_DISABLE		0x-----1--
			CACHE_FLUSH			0x-----2--
			CACHE_GETCTRL		0x-----4--

	[2] : SCHEDULE_CHANGE
			SCHEDULE_CHANGE		0x-------1
			
	[3]..[7] : Not Used 
*/

/* System call permition table 	*/
usr_syscall_perm_t usr_syscall_perm = {
% for p in range(0,num_parts):
	/* Partition ${p} */
	{  	<% value = 0x03 + (0xc  if p in fp_control else 0) %> ${hex(value)}
	, 	<% value = 0x1f + (0x60 if p in fp_control else 0) + (0x780 if p in cache_control else 0) %> ${hex(value)}
	, 	<% value = 1 if p in sched_control else 0 %> ${hex(value)}
	, 	0, 0, 0, 0, 0 		},
% endfor
};
	

	
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

