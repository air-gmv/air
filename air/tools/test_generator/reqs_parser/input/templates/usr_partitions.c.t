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
 * $Source: /IMADE/samples/samples/simple/config/usr_partitions.c,v $
 * $Branch:  $
 * $Revision: 1.3 $
 * $CommitId: 41c4e676f995d5f $
 * $Author: jaac $
 * $Date: 2011/09/07 13:20:26 $
 * ===========================================================================*/

#include <pmk_segregation.h>
#include <pmk_schedule.h>
#include <pmk_rtems.h>
#include <usr_partitions.h>
#include <pmk_linkcmd_vars.h>

/** Dynamic Memory Address Table type
 *  \note Holds the addresses of each section of memory of a given partition
 * 		 that can only be determined when a given partition boots up.
 */
typedef pmk_dyn_mem_map_t pmk_partition_dyn_mem_table_t [PMK_TOTAL_NUMBER_OF_PARTITIONS];
 
 
/* Number of partitions *******************************************	*/
/* having these as variables allows static linking of PMK lib		*/
/* Number of user partitions 	*/
uint32_t pmk_number_of_app_partitions 	= PMK_NUMBER_OF_APPLICATION_PARTITIONS;
/* Total number of partitions (system/pmk + user partitions) */
uint32_t pmk_total_number_of_partitions	= PMK_TOTAL_NUMBER_OF_PARTITIONS;


/** Default partition sizes table
 *  \note Uses default values from usr_partitions.h, but may be overridden by custom values here
 */
uint32_t pmk_partition_heap_size [PMK_TOTAL_NUMBER_OF_PARTITIONS] = {
	#ifndef __STRICT_ANSI__
	/* Cleaner way to initialize array, not ansi C tough (gcc specific extension) */
	[0 ... (PMK_NUMBER_OF_APPLICATION_PARTITIONS-1)] = PMK_DEFAULT_HEAP_SIZE_PARTITION
	#else
		#warning Please define the pmk_partition_heap_size array manually in here
		/* PMK_DEFAULT_HEAP_SIZE_PARTITION, PMK_DEFAULT_HEAP_SIZE_PARTITION, ... */
	#endif
};

/** Static Memory Address Table 
 *  \note Holds the addresses of each section of memory of a given partition
 * 		 that can be determined statically at PMK startup (before each partition boots up).
 */
pmk_stat_mem_map_t pmk_partition_static_mem_table [PMK_TOTAL_NUMBER_OF_PARTITIONS];

/** Plug-in Memory Address Table 
 *  \note Holds the addresses of plug-ins allocated memory
 *  \note The start of this zone is setup at the end of the static zone
 */
pmk_plugin_mem_map_t pmk_partition_plugin_mem_table = { &air_memmap_end, &air_memmap_end };

/** Dynamic Memory Address Table 
 *  \note Holds the addresses of each section of memory of a given partition
 * 		 that can only be determined when a given partition boots up.
 */
/* pmk_partition_dyn_mem_table_t * pmk_partition_dynamic_mem_table; */


/** Table of partitions 
  */
pmk_partition_t pmk_partition_table[PMK_TOTAL_NUMBER_OF_PARTITIONS] = {{ 0 }};

/** Table of partition permissions
  */
int pmk_part_perm_table[PMK_TOTAL_NUMBER_OF_PARTITIONS] = {
	% for p in range(0,num_parts):
		${supervisor_perm[p]},	/* P${p} */
	% endfor
};

/**
 * Per Partition Cache Activation Options:
 * The first 4 bits of each entry represent the cache state: the first two
 * bits indicate the intruction cache state and the other two the data cache 
 * state. The options for each pair of bits are the following: 
 *	X0	= disabled, 
 * 	01 	= frozen, 
 *  11 = enabled. 
 * As an example, if you want the both caches active
 * you should choose 0b1111 (0xF). For more infomation please refer to
 * the Leon manual.
 * The fifth bit defines if the cache's state can be changed during runtime.
 * If this last bit is 1, the cache options can be changed using syscalls.
 * Otherwise these options are frozen. 
 */
int pmk_part_cache_table[PMK_TOTAL_NUMBER_OF_PARTITIONS] = {
	% for p in range(0,num_parts):
	0x${str(cache_status[p][0]) + hex(cache_status[p][1] + cache_status[p][2])[2:]}, /* P${p} */
	% endfor
};

/**
 * Per Partition TOD clock syncronization options
 */
int pmk_part_tod_sync_table[PMK_TOTAL_NUMBER_OF_PARTITIONS] = {
	% for p in range(0,num_parts):
	${tod_sync[p]},	 /* P${p} */
	% endfor
};

/**
 * Per Partition local time options 
 */
int pmk_part_local_time_table[PMK_TOTAL_NUMBER_OF_PARTITIONS] = {
	% for p in range(0,num_parts):
	${local_time[p]},	 /* P${p} */
	% endfor
};

/* ************************************************************************* */
/** Reads the default partition size for the given partition from pmk_partition_heap_size
 *  \fn pmk_get_partition_heap_size
 *  \param [in]  partition_number	Size of stack to reserve
 *  \return 	selected partition heap size (from pmk_partition_heap_size table)
 *  \note 	this includes the size of the 'master PMK' partition
 */
 /* ************************************************************************* */
int pmk_get_partition_heap_size(int partition_number) {
	int partition_heap_size = -1; /* error value */
	/* input validation */
	if ((partition_number >= 0) && (partition_number <= PMK_NUMBER_OF_APPLICATION_PARTITIONS)) {
		/* retrieve value from table*/
		partition_heap_size = pmk_partition_heap_size[partition_number];
	}
	return partition_heap_size;
}

/* ************************************************************************* */
/** Fills the memory map with the static linking addresses for each partition
 *  \fn pmk_rtems_setup_mem_map_static_partition
 *  \param [in] partition_number partition to fill memory map
 *  \return pointer to relevant memory map section
 *  \warning This function depends heavily of the linkcmds linking script
 *  \warning Body of function is dependent of the number of partitions
 */
 /* ************************************************************************* */
pmk_stat_mem_map_t * pmk_rtems_setup_mem_map_static_partition(int partition_number){
	/* local pointer for easier coding	 	*/
	pmk_stat_mem_map_t * curr_mem_map = &(pmk_partition_static_mem_table[partition_number]);

	switch (partition_number) {
		
	% for p in range(0,num_parts):
		
		/* Partition ${p} :  ***************************************************	*/
		case ${p}: 	{
		
			/* overlays */
			/* partition init code */
			curr_mem_map->init_code_size 	= (int) ((char *)&air_memmap_part_init_text${p}_end -
													 (char *)&air_memmap_part_init_text${p}_load);
			curr_mem_map->init_code_load 	= (void *)&air_memmap_part_init_text${p}_load;	
			curr_mem_map->init_data_size 	= (int) ((char *)&air_memmap_part_init_data${p}_end -
													 (char *)&air_memmap_part_init_data${p}_load);
			curr_mem_map->init_data_load	= (void *)&air_memmap_part_init_data${p}_load;
			curr_mem_map->init_bss_size 	= (int) ((char *)&air_memmap_part_init_bss${p}_end -
													 (char *)&air_memmap_part_init_bss${p}_load);
			curr_mem_map->init_bss_load		= (void *)&air_memmap_part_init_bss${p}_load;
			
			/* pos */
			curr_mem_map->pos_data_load		= (void *)&air_memmap_pos_data${p}_load;
			curr_mem_map->pos_bss_load		= (void *)&air_memmap_pos_bss${p}_load;
		
			/* partition code section */
			curr_mem_map->part_code_start 	= &air_memmap_part_text${p}_start;
			curr_mem_map->part_code_size 	= ((char *)&air_memmap_part_text${p}_end - 
											   (char *)&air_memmap_part_text${p}_start);

			/* partition data section */
			curr_mem_map->part_data_start 	= &air_memmap_part_data${p}_start;
			curr_mem_map->part_data_size 	= ((char *)&air_memmap_part_data${p}_end - 
											   (char *)&air_memmap_part_data${p}_start);
			
			/* partition bss  section */
			curr_mem_map->part_bss_start 	= &air_memmap_part_bss${p}_start;
			curr_mem_map->part_bss_size 	= ((char *)&air_memmap_part_bss${p}_end - 
											   (char *)&air_memmap_part_bss${p}_start);
		} break;
	% endfor
		
		default: 	{	
		
			/* overlays */
			/* partition init code */
			curr_mem_map->init_code_load 	= NULL; 
			curr_mem_map->init_data_load	= NULL; 
			curr_mem_map->init_bss_load		= NULL;	
			
			/* pos */
			curr_mem_map->pos_data_load		= NULL;
			curr_mem_map->pos_bss_load		= 0;
		
			/* partition code section */
			curr_mem_map->part_code_start 	= NULL;
			curr_mem_map->part_code_size 	= 0;

			/* partition data section */
			curr_mem_map->part_data_start 	= NULL;
			curr_mem_map->part_data_size 	= 0;
			
			/* partition bss  section */
			curr_mem_map->part_bss_start 	= NULL;
			curr_mem_map->part_bss_size 	= 0;
		} break;
	}
	return curr_mem_map;
}

/* "Getters"  : not user modifiable ************************************************************** */

/* ************************************************************************* */
/** Gets pointer to pmk partition table
 *  \fn pmk_get_usr_partition_table
 *  \return pointer to partition table start (pmk_partition_t)
 */
 /* ************************************************************************* */
pmk_partition_t * pmk_get_usr_partition_table(){
	return &(pmk_partition_table[0]);
}

/* ************************************************************************* */
/** Gets number of user partitions (defined in usr_partitions.h)
 *  \fn pmk_get_number_of_app_partitions
 *  \return number of user partitions
 */
 /* ************************************************************************* */
uint32_t pmk_get_number_of_app_partitions(void){
	return pmk_number_of_app_partitions;
}

/* ************************************************************************* */
/** Gets total number of partitions (system/pmk + user partitions)  (defined in usr_partitions.h)
 *  \fn pmk_get_total_number_of_partitions
 *  \return total number of partitions
 */
 /* ************************************************************************* */
uint32_t pmk_get_total_number_of_partitions(void){
	return pmk_total_number_of_partitions;
}

/* ************************************************************************* */
/** Gets pointer to pmk static memory map
 *  \fn pmk_get_usr_static_memory_map
 *  \param [in] partition_number relevant memory map to return
 *  \return pointer to partition static memory map (pmk_stat_mem_map_t)
 */
 /* ************************************************************************* */
pmk_stat_mem_map_t * pmk_get_usr_static_memory_map(int partition_number){
	return (pmk_stat_mem_map_t *) &(pmk_partition_static_mem_table[partition_number]);
}

/* ************************************************************************* */
/** Gets pointer to pmk plugin memory map
 *  \fn pmk_get_usr_plugin_memory_map
 *  \return pointer to partition static memory map (pmk_plugin_mem_map_t)
 *  \warning unlike the static memory map, this one is shared among all partitions
 */
 /* ************************************************************************* */
pmk_plugin_mem_map_t * pmk_get_usr_plugin_memory_map(void){
	return (pmk_plugin_mem_map_t *) &(pmk_partition_plugin_mem_table);
}

/* ************************************************************************* */
/** Gets pointer to pmk dynamic memory map
 *  \fn pmk_get_usr_dynamic_memory_map
 *  \param [in] partition_number relevant memory map to return
 *  \return pointer to partition dynamic memory map (pmk_stat_mem_map_t)
 */
 /* ************************************************************************* */
pmk_dyn_mem_map_t * pmk_get_usr_dynamic_memory_map(int partition_number){
	pmk_partition_dyn_mem_table_t * dyn_mem_map_p;
	dyn_mem_map_p = (pmk_partition_dyn_mem_table_t *) &air_memmap_PMK_PartitionDynMemMap;
	return (pmk_dyn_mem_map_t *) &((*dyn_mem_map_p)[partition_number]);
}

/* ************************************************************************* */
/** Gets the RAM memory size
 *  \fn pmk_get_usr_ram_size
 */
 /* ************************************************************************* */
uint32_t * pmk_get_usr_ram_size(void){
	return (uint32_t *)PMK_AVAILABLE_MEMORY;
}

/* ************************************************************************* */
/** Get given partition permissions : supervisor or not supervisor
 *  \fn pmk_get_part_perm
 *  \param [in] part requested partition permissions
 */
 /* ************************************************************************* */
uint32_t pmk_get_part_perm(int part){
	if ((part >= 0) && (part < PMK_TOTAL_NUMBER_OF_PARTITIONS)) {
		return pmk_part_perm_table[part];
	} else {
		return 0;
	}
}

/* ************************************************************************* */
/** Get given partition cache settings : active or not active
 *  \param [in] part requested partition cahce options
 */
 /* ************************************************************************* */
uint32_t pmk_get_part_cache_opt(int part){
	if ((part >= 0) && (part < PMK_TOTAL_NUMBER_OF_PARTITIONS)) {
		return pmk_part_cache_table[part];
	} else {
		return 0;
	}
}

/* ************************************************************************* */
/** Get given partition time of day syncronization option : 
 *  active or not active
 *  \param [in] part requested sync options
 */
 /* ************************************************************************* */
uint32_t pmk_get_tod_sync_opt(int part){
	if ((part >= 0) && (part < PMK_TOTAL_NUMBER_OF_PARTITIONS)) {
		return pmk_part_tod_sync_table[part];
	} else {
		return 0;
	}
}

/* ************************************************************************* */
/** Get given partition time maintenance options:
 *  Partition global time(0) or local time (1)
 *  \param [in] part requested time maintenance options
 */
 /* ************************************************************************* */
uint32_t pmk_get_local_time_opt(int part){
	if ((part >= 0) && (part < PMK_TOTAL_NUMBER_OF_PARTITIONS)) {
		return pmk_part_local_time_table[part];
	} else {
		return 0;
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

