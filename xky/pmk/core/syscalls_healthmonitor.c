/**
 * @file
 * @author pfnf
 * @brief System calls related to health-monitor
 */

#include <pmk.h>
#include <multicore.h>
#include <segregation.h>
#include <health_monitor.h>


xky_status_code_e pmk_syscall_set_system_state(
        pmk_core_ctrl_t *core, xky_state_e state) {

    if (core->context->state != state &&
        state > XKY_STATE_MODULE_EXEC &&  state < XKY_STATE_PARTITION_HM) {

        core->context->state = state;
        return XKY_SUCCESSFUL;
    }

    return XKY_NO_ACTION;
}

xky_status_code_e pmk_syscall_get_hm_event(
        pmk_core_ctrl_t *core,
        xky_hm_event_t *event) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_hm_event_t *hm_event = (pmk_hm_event_t *)context->hm_event;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* create local copy of the current HM event */
    xky_hm_event_t l_event;
    l_event.error_id = hm_event->error_id;
    l_event.state_id = hm_event->state_id;

    /* remove core context */
    core_context_remove_hm_event(context);

    /* copy event to partition space */
    if (pmk_segregation_put_user(context, l_event, event) != 0) {
        return XKY_INVALID_POINTER;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return XKY_NO_ERROR;
}
