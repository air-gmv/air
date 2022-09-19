#include <printk.h>
#include <debugger.h>
#include <configurations.h>
#include <workspace.h>

void pmk_update_part_ticks(air_u8_t partition_id, air_u8_t core_id){
    
    pmk_partition_t *current_partition = &(air_shared_area.core[core_id].partition[partition_id]);

    partition_info[partition_id].last_ticks = &(current_partition->last_clock_tick);
    partition_info[partition_id].elapsed_ticks = &(current_partition->elapsed_ticks);
    partition_info[partition_id].time_spent = 0.0;

    return;
}

void pmk_change_debug_info(air_u8_t state, air_u64_t value){

    static air_u8_t last_part;
	
    last_part = pmk_global_trace->current_part;
    
    switch(state){
        case TICKS0: 
            partition_info[last_part].init_ticks = value;
	    break;
        case CURRENT_PART:
            pmk_global_trace->current_part = value;	    
      	    break;
        case CLOCK_INIT:
            pmk_global_trace->clock_init_value ++;
	    break;
        default:
            break;
    }

    return;
}

void pmk_init_part_array(air_u8_t number_of_parts){
   
    pmk_partition_t *current_partition = &(air_shared_area.core[0].partition[0]);

    air_u8_t part_count = 0;

    pmk_global_trace->partition_array = &(partition_info[0]); 
    pmk_global_trace->current_part = 0;
    pmk_global_trace->clock_init_value = 0;
    
    for(; part_count<number_of_parts; part_count++){
        partition_info[part_count].init_ticks = 0;
        partition_info[part_count].last_ticks = &(current_partition->last_clock_tick);
	partition_info[part_count].elapsed_ticks = &(current_partition->elapsed_ticks);
        partition_info[part_count].time_spent = 0.0;
    }

    return;
}

