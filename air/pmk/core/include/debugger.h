#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

#include <air.h>
//#include <workspace.h>

#define DEBUG
#define MAX_NUMBER_PARTITIONS 32

/**
 * @defgroup pmk_debugger PMK - Debugger
 * @brief Definitions, structures and functions to control the partitions state
 * @{
 */

typedef struct {
    air_clocktick_t init_ticks;
    air_clocktick_t *last_ticks;
    air_clocktick_t *elapsed_ticks;
    float time_spent;
} partition_info_t;

typedef struct {
    partition_info_t *partition_array;
    air_u8_t current_part;
    air_u16_t clock_init_value;
} pmk_global_trace_t;

partition_info_t partition_info[MAX_NUMBER_PARTITIONS];
pmk_global_trace_t *pmk_global_trace;

typedef enum {

	TICKS0        = 0,
	TICKS1        = 1,
	CURRENT_PART  = 2,
	CLOCK_INIT    = 3,

} array_position;

void pmk_update_part_ticks(air_u8_t partition_id, air_u8_t core_id);

void pmk_change_debug_info(air_u8_t state, air_u64_t value);

void pmk_init_part_array(air_u8_t number_of_parts);

//void pmk_print_current_part();

/** @} */

#endif /* __DEBUGGER_H__ */
