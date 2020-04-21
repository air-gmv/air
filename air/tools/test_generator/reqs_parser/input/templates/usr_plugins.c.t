/* ===========================================================================
 * This file contains information protected by Portuguese and international copyright law
 * ===========================================================================
 * (C) Skysoft Portugal SA, 2008-2010
 * ===========================================================================
 * Program  : PMK - Partition Management Kernel
 * Component: usr_configuration
 * Purpose  : User plug-in configuration
 * Status   : Stable
 * Level    : N/A
 * Author   : Joao Cristovao
 * ===========================================================================
 * Revision Control
 * ===========================================================================
 * $Source: /IMADE/samples/samples/control/config/usr_plugins.c,v $
 * $Branch:  $
 * $Revision: 1.3 $
 * $CommitId: ae44e0afe437c58 $
 * $Author: jaac $
 * $Date: 2011/06/29 10:28:19 $
 * ===========================================================================*/

#include <pmk_plugin.h>
#include <pal_callbacks.h>
#include <pal_pprintf.h>
#include <pal_ports.h>
#include <hal_leon_3.h>

/** PLUG-IN TYPE (PMK_PLUGIN_TYPE_QUEUE_PORT) ****************************	*/

%if qchannels:
% for c in range(0,len(qchannels)):
/* ---- Channel ${c} ---------------------------------------------------------	*/
/* Channel ${c} : control area 	*/
pmk_mem_area_perms_t chn${c}_control_area[] = {
/*	{part, permission_list , next					} */	
	{	${qchannels_links[c][0]}, PMK_MMU_ACC_RW, &(chn${c}_control_area[1])	},	
	{	${qchannels_links[c][1]}, PMK_MMU_ACC_RW, &(chn${c}_control_area[2])	},
<% size = len ( [i for i in parts_with_qports if i not in qchannels_links[c]]) %>
<% count = 0 %>
% for op in [i for i in parts_with_qports if i not in qchannels_links[c]]:
<% count += 1 %>
% if count < size:
	{   ${op}, PMK_MMU_ACC_R  , &(chn${c}_control_area[${count+2}]) },
% else:
	{   ${op}, PMK_MMU_ACC_R  , NULL },
	% endif
	% endfor
};


/* Channel 0 : data buffer (Transmit)	*/
pmk_mem_area_perms_t chn${c}_data_area_tx[] = {
/*	{part, permission_list , next					} */
	{	${qchannels_links[c][0]}, PMK_MMU_ACC_RW, &(chn${c}_data_area_tx[1])	},	
	/* TODO: with transport enabled, this disapears */
	{	${qchannels_links[c][1]}, PMK_MMU_ACC_RW , NULL	}	
};

/* Channel 0 : data buffer  (Receive)	*/ 	
/* TODO: will only be used with transport enabled */
pmk_mem_area_perms_t chn${c}_data_area_rx[] = {
/*	{part, permission_list , next					} */
	{	${qchannels_links[c][1]}, PMK_MMU_ACC_R, NULL	}
};

/* Channel 0 : Full memory map */
/* sizes are filled by init functions, and are therefor initialized to zero,
 * with the honorable exception of the port control area (for now)			*/
pmk_mem_area_t chn${c}_mem_area[] = {
/*	{  id, addr	, size	,  permission_list		, next 				}*/
	{	PMK_PLUGIN_PORT_CONTROL, NULL	, ${qport_ctl_size}	,
		&(chn${c}_control_area[0])	, &(chn${c}_mem_area[1])	},
	/* Transmitt	*/
	{	PMK_PLUGIN_PORT_QUEUE_OUT, NULL	, 0	  	,
		&(chn${c}_data_area_tx[0])	, &(chn${c}_mem_area[2])	},
	/* Receive	*/
	{	PMK_PLUGIN_PORT_QUEUE_IN, NULL	, 0	  	, 
		&(chn${c}_data_area_rx[0])	, NULL			 	}
};
% endfor

% endif

/* ---- Printf ---------------------------------------------------------	*/
pmk_mem_area_perms_t printf_control_area[] = {
/*	{part, permission_list , next					} */	
% for p in range(0,len(parts_with_printf)):
	% if p < len(parts_with_printf) -1:
	{	${parts_with_printf[p]}, PMK_MMU_ACC_RW, &(printf_control_area[${p+1}])	},	
	% else:
	{	${parts_with_printf[p]}, PMK_MMU_ACC_RW, NULL }
	% endif
% endfor
};

pmk_mem_area_t printf_mem_area[] = {
/*	{  id, addr	, size	, permission_list			, next 				}*/
	{	0, NULL	, ${printf_mem_size}, &(printf_control_area[0])	, NULL	}
};

/* ---- pprintf ---------------------------------------------------------	*/
/* pprintf is a faster printf, that relays all output to a specialized 
 * 'printf' partition, that should be the last one to be scheduled in a MTF */
% for p in range(0,len(parts_with_pprintf)-1):
pmk_mem_area_perms_t pprintf_area_${p}[] = {
/*	{part, permission_list , next					} */	
	{	${parts_with_pprintf[p]}, PMK_MMU_ACC_RW, &(pprintf_area_${p}[1])	},	
	{	${last_partition}, PMK_MMU_ACC_RW, NULL }
};
% endfor

pmk_mem_area_t pprintf_mem_area[] = {
/*	{  id, addr	, size	, permission_list			, next 				}*/
% for p in range(0,len(parts_with_pprintf)-1):
	% if p < len(parts_with_pprintf) - 2:
	{	${p}	, NULL	, ${pprintf_mem_size}, &(pprintf_area_${p}[0]), &(pprintf_mem_area[${p+1}])	},
	% else:
	{	${p}	, NULL	, ${pprintf_mem_size}, &(pprintf_area_${p}[0]), NULL	}
	% endif
% endfor
};

/* ---- TIMER1 ---------------------------------------------------------	*/
/* TODO: 4KB shared among all partitions for a single int containing the 
			last clock tick */
% if gmv_tests:
pmk_mem_area_perms_t timer1_perms[] = {
/*	{part, permission_list , next					} */	
    % for p in range(0,len(parts_with_pprintf)-1):
	{	${parts_with_pprintf[p]}, PMK_MMU_ACC_RW, &timer1_perms[${p+1}]	},
	% endfor
	{   ${parts_with_pprintf[p+1]},PMK_MMU_ACC_RW, NULL }
};

pmk_mem_area_t timer1_mem_area[] = {
/*	{  id, addr	, size	, permission_list			, next 				}*/
	{	0	, NULL	, 4096, &(timer1_perms[0]), NULL	},
};
% endif

/** Complete plugin list **************************************************	*/
/* one plugin for each channel, even if the same type (queueing or sampling)*/
pmk_plugin_t plugin_list [] = {
% if qchannels:
% for c in range(0,len(qchannels)):
	{	${qchannels[c]}				,	/* id : for ports this is the channel	*/
		PMK_PLUGIN_TYPE_QUEUE_PORT	, 	/* type									*/
		PMK_PLUGIN_SUPERUSER		, 	/* mode									*/
		pal_ports_init_sizes		, 	/* init each memory area size			*/
		pal_ports_init_addrs		,  	/* fill each memory area addr			*/
		PMK_CALLBACK_NONE,
		NULL						,	/* pre_part func: N/A					*/
		pal_port_refresh			,	/* pos_part func: port refresh call		*/
		NULL						, 	/* auxiliary pointer					*/
		&(chn${c}_mem_area)[0]		, 	/* mem_area								*/
		&(plugin_list[${c+1}])				/* pointer	to next						*/
	},
% endfor
% else:
<% c = -1 %>
% endif

% if apex == "AIRPEX":
	/* AIRPEX TICK	*/
	<% c += 1  %>
	{	${c}					,	
		PMK_PLUGIN_TYPE_AIRPEX		, 	/* type									*/
		PMK_PLUGIN_SUPERUSER		, 	/* mode									*/
		NULL						, 	/* pre_init 	: N/A					*/
		NULL						,  	/* pos_init 	: N/A					*/
		PMK_CALLBACK_AIRPEX_TICK	,	
		NULL						,	/* pre_part func: N/A					*/
		NULL						,	/* pos_part func: N/A					*/
		NULL						, 	/* auxiliary pointer					*/
		NULL						, 	/* mem_area		: N/A					*/
		&(plugin_list[${c+1}]) 			/* pointer	to next						*/
	},
% endif
%if len(parts_with_printf) > 0:
	<% c += 1  %>
	{	${c}						,	
		PMK_PLUGIN_PRINTF			, 	/* type									*/
		PMK_PLUGIN_USER				, 	/* mode									*/
		NULL						, 	/* pre_init 	: N/A					*/
		NULL						,  	/* pos_init 	: N/A					*/
		PMK_CALLBACK_NONE			,	
		NULL						,	/* pre_part func: N/A					*/
		NULL						,	/* pos_part func: N/A					*/
		NULL						, 	/* auxiliary pointer					*/
		&(printf_mem_area)[0]		, 	/* mem_area		: N/A					*/
	% if len(parts_with_pprintf) > 0:
		&(plugin_list[${c+1}]) 
	% else:
		NULL 							/* pointer	to next						*/
	% endif
	},
% endif
%if len(parts_with_pprintf) > 0:
	<% c += 1  %>
	{	${c}						,	
		PMK_PLUGIN_PPRINTF			, 	/* type									*/
		PMK_PLUGIN_USER				, 	/* mode									*/
		NULL						, 	/* pre_init 	: N/A					*/
		pprintf_init				,  	/* copy the addresses to internal struct*/
		PMK_CALLBACK_NONE			,	
		NULL						,	/* pre_part func: N/A					*/
		pal_pprint_enter_part		,	/* go to next execution window log		*/
		NULL						, 	/* auxiliary pointer					*/
		&(pprintf_mem_area)[0]		, 	/* mem_area		: N/A					*/
	% if gmv_tests:
		&(plugin_list[${c+1}]) 
	% else:
		NULL 							/* pointer	to next						*/
	% endif
	},
% endif
%if gmv_tests:
	<% c += 1  %>
	{	${c}						,	
		PMK_PLUGIN_TIMER1			, 	/* type									*/
		PMK_PLUGIN_USER				, 	/* mode									*/
		NULL						, 	/* pre_init 	: N/A					*/
		hal_activate_timer1			,  	/* copy the addresses to internal struct*/
		PMK_CALLBACK_NONE			,	
		NULL						,	/* pre_part func: N/A					*/
		NULL						,	/* go to next execution window log		*/
		NULL						, 	/* auxiliary pointer					*/
		&(timer1_mem_area)[0]		, 	/* mem_area		: N/A					*/
		NULL 							/* pointer	to next						*/
	},
% endif
};

/** Returns the user plugins control structure
 *  \fn get_usr_plugins
 *  \return pmk_plugin_t pointer
 */
pmk_plugin_t * get_usr_plugins(void) {
	return &(plugin_list[0]);
}

/** Returns the user plugins first memory area
 *  \fn get_usr_plugins_fst_memarea
 *  \return void * pointer
 */
void * get_usr_plugins_fst_memarea(pmk_plugin_type_e plugin_type) {
	int i;
	for (	i = 0;
			(i < sizeof (plugin_list)) && (plugin_list[i].type != plugin_type);
			i++																);
	return plugin_list[i].mem_area_list->addr;
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

