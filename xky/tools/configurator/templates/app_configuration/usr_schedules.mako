<%
    import air
    import utils.templates as makoutils
    import parsers.a653.definitions as a653_definitions

    ## convert nanoseconds to ticks
    def nsec_to_ticks(value):
        return int((value / a653_definitions.A653_TIMESCALE) * configuration.ticks_per_second)
%>\
${makoutils.cfileHeader('usr_schedules.c', 'Module Schedules configuration')}

#include <schedule.h>
#include <partition.h>
#include <usr_config.h>

/**
 * @brief Partition table (defined in usr_partitions.c)
 */
extern pmk_partition_t usr_partition_table[USR_PARTITIONS];

% for i, schedule in enumerate(configuration.schedules):
/**
 * @brief Schedule ${i}: scheduled partitions
 */
static pmk_partition_schedule_t usr_schedule${i}_partitions[${len(schedule.partitions)}] = ${'\\'}
{
% for j, partition_schedule in enumerate(schedule.partitions):
    {   /* ${partition_schedule} */
	    .partition    = &usr_partition_table[${partition_schedule.partition.index}],
	    .set_schedule = ${"1" if partition_schedule.set_schedule else "0"},
	    .action       = PMK_SCHED_CHANGE_ACTION_${partition_schedule.change_action},
	    .period       = ${nsec_to_ticks(partition_schedule.period)}ULL,
	    .duration     = ${nsec_to_ticks(partition_schedule.duration)}ULL,
    }${"," if j < len(schedule.partitions) - 1 else ""}
% endfor
};

% for j in range(0, configuration.core_count):
/**
 * @brief Schedule ${i}, core ${j}: windows
 */
static pmk_window_t usr_schedule${i}_windows${j}[${len(schedule.chain_of_execution[j])}] = ${'\\'}
{
% for k, window in enumerate(schedule.chain_of_execution[j]):
    {   /* ${window} */
        .duration     = ${nsec_to_ticks(window.duration)},
        .period_start = ${'1' if window.period_start else '0'},
        % if not window.partition:
        .partition    = NULL,
        %else:
        .partition    = &usr_partition_table[${window.partition.index}],
        % endif
        .virtual_core = ${window.cores.index(j)},
    }${"," if k < len(schedule.chain_of_execution[j]) - 1 else ""}
% endfor
};
% endfor

/**
 * @brief Schedule ${i}: cores schedules
 */
static pmk_core_schedule_t usr_schedule${i}_cores[USR_CORES] = ${'\\'}
{
% for j in range(0, configuration.core_count):
    { .count = ${len(schedule.chain_of_execution[j])}, .windows = (void *)usr_schedule${i}_windows${j} }${"," if j < configuration.core_count - 1 else ""}
% endfor
};
% endfor

/**
 * @brief Module Schedules Configuration
 */
pmk_schedule_t usr_schedule_table[USR_SCHEDULES] = ${'\\'}
{
% for i, schedule in enumerate(configuration.schedules):
    {   /* ${schedule} */
        .id         = ${schedule.id},
        .name       = "${schedule.name}",
        .mtf        = ${nsec_to_ticks(schedule.mtf)},
        .initial    = ${"1" if schedule.is_initial else "0"},
        .partitions = { .length =  ${len(schedule.partitions)}, .elements = (void *)usr_schedule${i}_partitions },
        .cores      = usr_schedule${i}_cores,
    }${"," if i < len(configuration.partitions) - 1 else ""}
% endfor
};
