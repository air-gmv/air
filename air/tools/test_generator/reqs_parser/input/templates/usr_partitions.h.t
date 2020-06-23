/* ===========================================================================
 * This file contains information protected by Portuguese and international copyright law
 * ===========================================================================
 * (C) Skysoft Portugal SA, 2008-2010
 * ===========================================================================
 * Program  : PMK - Partition Management Kernel
 * Component: usr_configuration
 * Purpose  : User partition configuration 
 * Status   : Stable
 * Level    : N/A
 * Author   : Joao Cristovao
 * ===========================================================================
 * Revision Control
 * ===========================================================================
 * $Source: /IMADE/samples/samples/control/config/usr_partitions.h,v $
 * $Branch:  $
 * $Revision: 1.2 $
 * $CommitId: d304dfb75eb4347 $
 * $Author: jaac $
 * $Date: 2011/06/17 15:42:36 $
 * ===========================================================================*/

/* MAKO VARIABLES USED */
/*
	available_ram, num_parts, default_heap
 */
 
 /** \brief System available RAM memory */
#define PMK_AVAILABLE_MEMORY (${hex(available_ram)})
 
/* Number of partitions *******************************************	*/
/* Number of user partitions 	*/
#define PMK_NUMBER_OF_APPLICATION_PARTITIONS 	(${num_parts})
/* Shorthand notation 			*/
#define PMK_NUM_PART 							(PMK_NUMBER_OF_APPLICATION_PARTITIONS) 
/* Total number of partitions (system/pmk + user partitions) */
#define PMK_TOTAL_NUMBER_OF_PARTITIONS			(PMK_NUM_PART)


/* Default heap sizes 	*******************************************	*/
/* Partition default heap size */
#define PMK_DEFAULT_HEAP_SIZE_PARTITION		(${hex(default_heap)})


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
