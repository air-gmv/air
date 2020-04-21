#include <pmk_hm.h>
#include <pal_util.h>



/* Health Monitor table pointer */
pmk_hm_ctrl_t * hm_ctrl = NULL;


/* Init user health monitor table */
void init_usr_hm_table(void) {
	hm_ctrl = &(pmk_get_shared_area_pointer()->hm_table);


	/* System HM table ******************************************** */
% for systab_state in range(0,len(sys_states)):
	hm_ctrl->systab[${systab_state}].state_id = ${systab_state};
	
	% for systab_level in range(0,len(sys_err_levels[systab_state])):

		hm_ctrl->systab[${systab_state}].lvl_err[${systab_level}].error_id		 = ${sys_err_levels[systab_state][systab_level][0]};
		hm_ctrl->systab[${systab_state}].lvl_err[${systab_level}].level_id		 = ${sys_err_levels[systab_state][systab_level][1]};
		hm_ctrl->systab[${systab_state}].lvl_err[${systab_level}].app_error_code = ${sys_err_levels[systab_state][systab_level][2]};
	% endfor
	
% endfor
	
	/* Module HM table ******************************************** */
% for modtab_state in range(0,len(mod_states)):
	hm_ctrl->modtab[${modtab_state}].state_id = ${mod_states[modtab_state][1]};
	
	% for error_id in range(0,len(mod_err_ids[modtab_state])):

		hm_ctrl->modtab[${modtab_state}].error_action[${error_id}].error_id		 = ${mod_err_ids[modtab_state][error_id][0]};
		hm_ctrl->modtab[${modtab_state}].error_action[${error_id}].action_id	 = ${mod_err_ids[modtab_state][error_id][1]};
	% endfor
	
% endfor

	/* Partition tables */
% for p in partitions:
% for partab_state in range(0,len(par_states)):
	hm_ctrl->parttab[${p}][${partab_state}].state_id = ${par_states[partab_state][1]};

	% for error_id in range(0,len(par_err_ids[p][partab_state])):

		hm_ctrl->parttab[${p}][${partab_state}].error_action[${error_id}].error_id	 = ${par_err_ids[p][partab_state][error_id][0]};
		hm_ctrl->parttab[${p}][${partab_state}].error_action[${error_id}].action_id	 = ${par_err_ids[p][partab_state][error_id][1]};
	% endfor	


% endfor
% endfor
	
}
