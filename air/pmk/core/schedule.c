/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file schedule.c
 * @author pfnf
 * @brief Partition scheduling functions
 */

#include <pmk.h>
#include <bsp.h>
#include <rvs.h>
#include <tod.h>
#include <error.h>
#include <printk.h>
#include <barrier.h>
#include <schedule.h>
#include <partition.h>
#include <multicore.h>
#include <workspace.h>
#include <configurations.h>

/**
 * @brief Module schedule control
 */
static pmk_schedule_ctrl_t scheduler_ctrl;

/**
 * @brief
 */
pmk_schedule_t *pmk_schedule_list;


void pmk_scheduler_init(void) {

	int i;
	pmk_schedule_t *initial_schedule = NULL;

#ifdef PMK_DEBUG
    printk(" :: Scheduler initialization\n");
#endif

	air_shared_area.schedule_ctrl = &scheduler_ctrl;

	/* find the first schedule (multiple scheduler case) */
	pmk_list_t *list = pmk_get_schedules();
	pmk_schedule_list = (pmk_schedule_t *)list->elements;

	if (list->length > 1) {

	    /* for a configuration with MMS */
	    for (i = 0; i < list->length; ++i) {
	        if (1 == pmk_schedule_list[i].initial) {
	            initial_schedule = &pmk_schedule_list[i];
	        }
	    }

	} else if (list->length == 1) {

	    /* for a configuration without MMS */
	    initial_schedule = &pmk_schedule_list[0];

	} else {

	    /* invalid configuration */
	    pmk_fatal_error(
	            PMK_INTERNAL_ERROR_CONFIG, __func__, __FILE__, __LINE__);
	}


	/* check if an initial schedule was found */
	if (NULL == initial_schedule) {
	    pmk_fatal_error(
	            PMK_INTERNAL_ERROR_CONFIG, __func__, __FILE__, __LINE__);
	}

	/* initialize the MTF synchronization barrier */
	pmk_barrier_init(
	        &scheduler_ctrl.mtf_barrier, air_shared_area.configured_cores);

	/* force a schedule change on the next clock tick */
	scheduler_ctrl.mtf_ticks = 0;
	scheduler_ctrl.total_ticks = ~0;
    scheduler_ctrl.curr_schedule = NULL;
    scheduler_ctrl.last_switch_ticks = 0;
    scheduler_ctrl.next_schedule = initial_schedule;
    for (i = 0; i < air_shared_area.configured_cores; ++i) {
        air_shared_area.core[i].schedule = NULL;
        air_shared_area.core[i].partition_switch = 1;
        air_shared_area.core[i].next_switch_ticks = 0;
    }

#ifdef PMK_DEBUG
    pmk_workspace_debug();
#endif

    return;
}

void pmk_apply_next_schedule(pmk_core_ctrl_t *core) {

    air_u32_t i, j;
    pmk_partition_t *partition;
    pmk_partition_schedule_t *partition_schedule;

    /* update last schedule switch ticks */
    scheduler_ctrl.last_switch_ticks = scheduler_ctrl.total_ticks;

    /* clear current partition scheduler information */
    if (scheduler_ctrl.curr_schedule != NULL) {

        for (i = 0; i < scheduler_ctrl.curr_schedule->partitions.length; ++i) {

            partition_schedule =
                    &((pmk_partition_schedule_t *)scheduler_ctrl.curr_schedule->partitions.elements)[i];
            partition = partition_schedule->partition;

            partition->period = 0;
            partition->duration = 0;
        }
    }

    /* apply changes to the partitions */
    for (i = 0; i < scheduler_ctrl.next_schedule->partitions.length; ++i) {

        partition_schedule =
                &((pmk_partition_schedule_t *)scheduler_ctrl.next_schedule->partitions.elements)[i];
        partition = partition_schedule->partition;

        /* set the partition configurations */
        partition->period = partition_schedule->period;
        partition->duration = partition_schedule->duration;

        /* set scheduler change event */
        partition->events |= AIR_EVENT_SCHEDULE_CHANGE;

        /* apply schedule change action */
        if (partition_schedule->action > PMK_SCHED_CHANGE_ACTION_IGNORE)
        {
            /* Only apply action for previously started partition in NORMAL mode*/
            if(AIR_MODE_NORMAL == partition->mode && partition->state == PMK_PARTITION_STATE_RUNNING)
            {
                /* clear contexts */
                for (j = 0; j < partition->cores; ++j) {

                    core_context_setup_idle(&partition->context[j]);
                }

                switch (partition_schedule->action) {

                    /* change partition to IDLE */
                    case PMK_SCHED_CHANGE_ACTION_IDLE:
                        partition->start_condition = AIR_START_CONDITION_NORMAL;
                        partition->state = PMK_PARTITION_STATE_HALTED;
                        partition->mode = AIR_MODE_IDLE;
                        break;

                    /* change partition to cold start */
                    case PMK_SCHED_CHANGE_ACTION_COLD_START:
                        partition->start_condition = AIR_START_CONDITION_NORMAL;
                        partition->state = PMK_PARTITION_STATE_RESTARTING;
                        partition->mode = AIR_MODE_COLD_START;
                        core_context_setup_reload_partition(&partition->context[0], partition);
                        break;

                    /* change partition to warm start */
                    case PMK_SCHED_CHANGE_ACTION_WARM_START:
                        partition->start_condition = AIR_START_CONDITION_NORMAL;
                        partition->state = PMK_PARTITION_STATE_RESTARTING;
                        partition->mode = AIR_MODE_WARM_START;
                        core_context_setup_reload_partition(&partition->context[0], partition);
                        break;

                    /* default case, no action */
                    default:
                        break;
                }
            }
        }

        /* apply schedule change permission */
        if (1 == partition_schedule->set_schedule || AIR_PERMISSION_SUPERVISOR == partition->permissions) {

            partition_schedule->partition->permissions |=
                    AIR_PERMISSION_SET_SCHEDULE;

        } else {

            partition_schedule->partition->permissions &=
                    ~AIR_PERMISSION_SET_SCHEDULE;
        }
    }

    /* apply the schedule to each core */
    for (i = 0; i < air_shared_area.configured_cores; ++i) {

        air_shared_area.core[i].schedule =
                &scheduler_ctrl.next_schedule->cores[i];
    }

    /* apply the new schedule */
    scheduler_ctrl.curr_schedule = scheduler_ctrl.next_schedule;
    return;
}

void pmk_partition_scheduler(void *isf, pmk_core_ctrl_t *core) {

    air_u32_t vcore_id;
    pmk_partition_t *partition;
    pmk_core_schedule_t *core_schedule = core->schedule;

    /* update core (per core) */
    if (0 == core->idx) {
        ++scheduler_ctrl.mtf_ticks;
        ++scheduler_ctrl.total_ticks;
        pmk_tod_update();
        bsp_interrupt_broadcast(BSP_IPC_PCS);
    }

#ifdef TEST_CLOCK_TICK_PERF
    /* begin: clock tick measurement */
    RVS_Ipoint((core->idx  << 6) | (0 << 4) | (0 << 1) | 0);
#endif

#ifdef TEST_POS_CLOCK_TICK_PERF
    /* begin: clock tick measurement (ends in POS) */
    RVS_Ipoint((core->idx  << 6) | (0 << 4) | (0 << 1) | 0);
#endif

    /* sync all cores */
    pmk_barrier_wait(&scheduler_ctrl.mtf_barrier, core->idx);

    /* update partition elapsed ticks */
    if (core->partition != NULL && core_context_id(core->context) == 0) {
        core->partition->events |= AIR_EVENT_CLOCKTICK;
        ++core->partition->elapsed_ticks;
    }

    /* partition context switch is required*/
    if (scheduler_ctrl.mtf_ticks >= core->next_switch_ticks) {

#ifdef TEST_CONTEXT_SWITCH_PERF
        /* begin: context switch measurement */
        RVS_Ipoint((core->idx  << 6) | (3 << 4) | (0 << 1) | 0);
#endif

        /* flag the switch */
        core->partition_switch = 1;

        /* save the current context if necessary */
        if (core->context != NULL) {

            /* get the partition control structure */
            partition = core->partition;

            /* get the current virtual core */
            air_u32_t vcore_id = core_context_id(core->context);

            /* store the current core partition context */
            if (!core_context_trashed(core->context)) {
                core_context_save(core);
            }

            /* if the core is running a partition */
            if (partition != NULL) {

                /* store last ticks */
                partition->last_clock_tick = scheduler_ctrl.total_ticks;

                /* un-map the current core */
                partition->core_mapping[vcore_id] = PMK_MAX_CORES;
            }
        }

        /* check if this switch is the end of a MTF */
        if (core_schedule == NULL || core->preempt_point == core_schedule->count) {

            /* allow the master core to change the schedule */
            if (core->idx  == 0) {

                /* check if a schedule switch is necessary */
                if (scheduler_ctrl.curr_schedule != scheduler_ctrl.next_schedule) {

                    /* apply next schedule */
                    pmk_apply_next_schedule(core);
                }
            }

            /* sync a possible schedule change */
            while(scheduler_ctrl.curr_schedule != scheduler_ctrl.next_schedule);

            /* reset to window 0 */
            scheduler_ctrl.mtf_ticks = 0;
            core->preempt_point = 0;
            core->next_switch_ticks = 0;
        }

        /* get the current core schedule and current window*/
        core_schedule = core->schedule;
        air_u32_t wnd_idx = core->preempt_point;

        /* determine when the core will perform the next context switch */
        core->next_switch_ticks += core_schedule->windows[wnd_idx].duration;

        /* get core partition */
        partition = core_schedule->windows[wnd_idx].partition;
        core->partition = partition;

        /* setup the partition context */
        if (partition != NULL) {

#ifdef TEST_SCHEDULING_CORR
            /* end: context switch measurement */
            RVS_Ipoint((core->idx  << 6) | (2 << 4) | ((partition->idx) << 1) | 0);
#endif

            /* set MTF begin flag */
            if (wnd_idx == 0) {
                partition->events |= AIR_EVENT_SCHEDULE_START;
            }

            /* set period start flag */
            if (core_schedule->windows[wnd_idx].period_start == 1) {
                partition->events |= AIR_EVENT_PERIOD_START;
            } else {
                partition->events &= ~AIR_EVENT_PERIOD_START;
            }

            /* set partition's current window identifier */
            partition->window_id = core_schedule->windows[wnd_idx].id;

            /* set window start flag */
            partition->events |= AIR_EVENT_WINDOW_START;

            /* assign the partition virtual core to this core */
            vcore_id = core_schedule->windows[wnd_idx].virtual_core;

            /* wait for the core context to be released */
            while (partition->core_mapping[vcore_id] != PMK_MAX_CORES);

            /* get the new context and map the virtual core to a real core */
            core->context = &partition->context[vcore_id];
            partition->core_mapping[vcore_id] = core->idx;

            /* check if partition is starting */
            if (partition->state == PMK_PARTITION_STATE_INIT ||
                partition->state == PMK_PARTITION_STATE_NOT_RUN) {

               pmk_partition_start(partition, core->context);
               partition->last_clock_tick = scheduler_ctrl.total_ticks;
            }

            /* update partition global time */
            if (core_context_id(core->context) == 0) {

                if ((partition->permissions & AIR_PERMISSION_GLOBAL_TIME) != 0) {

                    air_u64_t dt = scheduler_ctrl.total_ticks - partition->last_clock_tick;
                    partition->elapsed_ticks += dt;
                }
            }

            /* sync SMP cores */
            pmk_barrier_wait(&partition->smp_barrier, vcore_id);


        /* setup the idle context */
        } else {

            core->context = &pmk_core_idle_context[core->idx];
        }

        /* restore the context */
        core_context_restore(core);

        /* advance to the next window */
        ++core->preempt_point;

#ifdef TEST_CONTEXT_SWITCH_PERF
        /* end: context switch measurement */
        RVS_Ipoint((core->idx  << 6) | (3 << 4) | (0 << 1) | 1);
#endif
    }

    /* end of partition context switch */
#ifdef TEST_CLOCK_TICK_PERF
        /* end: clock tick measurement */
        RVS_Ipoint((core->idx  << 6) | (0 << 4) | (0 << 1) | 1);
#endif
}

pmk_schedule_t *pmk_get_schedule_by_id(air_identifier_t sid) {

    air_u32_t i;
    pmk_schedule_t *found = NULL;
    pmk_list_t *list = pmk_get_schedules();
    for(i = 0; i < list->length; ++i){
        pmk_schedule_t *schedule = pmk_get_from_list(pmk_schedule_t, list, i);
        if (sid == schedule->id) {
            found = schedule;
        }
    }
    return found;
}

pmk_schedule_t *pmk_get_schedule_by_name(air_name_ptr_t name) {

    air_u32_t i;
    pmk_schedule_t *found = NULL;
    pmk_list_t *list = pmk_get_schedules();
    for(i = 0; i < list->length; ++i){
        pmk_schedule_t *schedule = pmk_get_from_list(pmk_schedule_t, list, i);
        if (strncmp(name, schedule->name, sizeof(air_name_t)) == 0) {
            found = schedule;
        }
    }
    return found;
}
