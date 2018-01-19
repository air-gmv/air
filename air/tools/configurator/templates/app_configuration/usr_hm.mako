<%
    import air
    import utils.templates as makoutils
    import parsers.a653.definitions as a653_definitions

    def get_error_level(state_id, error_id):

        # get system state
        level = 'UNDEFINED'
        system_state = [ s for s in configuration.system_hm_table if s.id == state_id ]
        if system_state:
            error = [ e for e in system_state[0].errors if e.id == error_id ]
            if error: level = error[0].level

        return "PMK_ERROR_LEVEL_{0}".format(level)

    def get_error_action(system_state, error_id):

        # get error action
        action = 'UNDEFINED'
        error = [ e for e in system_state.errors if e.id == error_id ]
        if error: action = error[0].action
        return "PMK_ACTION_{0}".format(action)

%>\
${makoutils.cfileHeader('usr_hm.c', 'Health-Monitor configuration')}

#include <health_monitor.h>

/**
 * @brief Health-Monitor System State Table
 */
pmk_hm_level_list_t usr_hm_system_table[PMK_HM_STATES_COUNT] = ${'\\'}
{
% for i in range(air.MIN_STATE_ID, air.MAX_STATE_ID):
    {   /* ${a653_definitions.SYSTEM_STATE_STR.format(i)} */
    % for j in range(air.MIN_ERROR_ID, air.MAX_ERROR_ID):
        ${get_error_level(i, j)}${"," if j < air.MAX_ERROR_ID - 1 else ""}
    % endfor
    }${"," if i < air.MAX_STATE_ID - 1 else ""}
% endfor
};

/**
 * @brief Module Health-Monitor table entries
 */
static pmk_hm_action_list_t usr_hm_module_entries[${len(configuration.module_hm_table)}] = ${'\\'}
{
% for i, system_state in enumerate(configuration.module_hm_table):
    {   /* ${system_state} */
    % for j in range(air.MIN_ERROR_ID, air.MAX_ERROR_ID):
        ${get_error_action(system_state, j)}${"," if j < air.MAX_ERROR_ID - 1 else ""}
    % endfor
    }${"," if i < len(configuration.module_hm_table) - 1 else ""}
% endfor
};

/**
 * @brief Health-Monitor Module State Table
 */
pmk_hm_action_list_t *usr_hm_module_table[PMK_HM_STATES_COUNT] = ${'\\'}
{
<% j = 0 %>\
% for i in range(air.MIN_STATE_ID, air.MAX_STATE_ID):
<%
    state_defined = [ s for s in configuration.module_hm_table if s.id == i  ]
%>\
    % if state_defined:
    &usr_hm_module_entries[${j}]\
<% j += 1 %>\
    % else:
    NULL\
    % endif
${"," if i < air.MAX_STATE_ID - 1 else ""}
% endfor
};
%for i, partition in enumerate(configuration.partitions):

/**
 * @brief Partition ${i} Health-Monitor Table entries
 */
pmk_hm_action_list_t usr_hm_partition${i}_entries[${len(partition.hm_table)}] = ${'\\'}
{
    % for j, system_state in enumerate(partition.hm_table):
    {   /* ${system_state} */
        % for k in range(air.MIN_ERROR_ID, air.MAX_ERROR_ID):
        ${get_error_action(system_state, k)}${"," if k < air.MAX_ERROR_ID - 1 else ""}
        % endfor
    }${"," if j < len(partition.hm_table) - 1 else ""}
    % endfor
};

/**
 * @brief Partition ${i} Health-Monitor Table
 */
pmk_hm_action_list_t *usr_hm_partition${i}_table[PMK_HM_STATES_COUNT] = ${'\\'}
{
<% k = 0 %>\
    % for j in range(air.MIN_STATE_ID, air.MAX_STATE_ID):
<%
    state_defined = [ s for s in partition.hm_table if s.id == j  ]
%>\
        % if state_defined:
    &usr_hm_partition${i}_entries[${k}]\
<% k += 1 %>\
        % else:
    NULL\
        % endif
${"," if j < air.MAX_STATE_ID - 1 else ""}
    % endfor
};
%endfor