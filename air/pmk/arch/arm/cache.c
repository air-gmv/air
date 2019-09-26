/*
 * Copyright (C) 2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file cache.c
 * \author lumm
 * \brief Cache clean and invalidate functions
 */

#include <cache.h>
#include <arm_cp15.h>

void arm_instruction_cache_invalidate(void) {
    arm_cp15_instruction_cache_invalidate();
}

void arm_data_cache_invalidate_all_levels(void) {
    air_u32_t clidr = arm_cp15_get_cache_level_id();
    air_u32_t loc = (clidr >> 24U) & 0b111;
    air_u32_t level = 0;

    for (level = 0; level < loc; ++level) {
        air_u32_t ctype = (clidr >> (3U * level)) & 0b111;

        /* Check if this level has a data cache or unified cache */
        if (((ctype & (0x6)) == 2) || (ctype == 4)) {
            arm_cache_invalidate_level(level, CP15_CSSELR_DATA);
        }
    }
}

void arm_data_cache_clean_all_levels(void) {
    air_u32_t clidr = arm_cp15_get_cache_level_id();
    air_u32_t loc = (clidr >> 24U) & 0b111;
    air_u32_t level = 0;

    for (level = 0; level < loc; ++level) {
        air_u32_t ctype = (clidr >> (3U * level)) & 0b111;

        /* Check if this level has a data cache or unified cache */
        if ( (ctype == 0b010) || (ctype == 0b100) ) {
            arm_data_cache_clean_level(level);
        }
    }
}

/**
 * \todo
 * TODO in RTEMS they disable interrupts for some reason
 */
air_u32_t arm_cache_get_size_id_for_level(air_u32_t level) {

    air_u32_t ccsidr;

    arm_cp15_set_cache_size_selection(level);
    arm_instruction_synchronization_barrier();
    ccsidr = arm_cp15_get_cache_size_id();

    return ccsidr;
}

/**
 * \details
 * L = log2(#words * 4(32bits))
 * LineSize = log(#words) - 2
 * => L = log2(2^(LineSize + 2) * 4)
 */
air_u32_t arm_cache_get_line_shift(air_u32_t ccsidr) {
    return (ccsidr & 0x7) + 4;
}

/**
 * \details
 * Associativity + 1
 */
air_u32_t arm_cache_get_associativity(air_u32_t ccsidr) {
    return ((ccsidr >> 3U) & 0x3ff) + 1;
}

/**
 * \details
 * NumSets + 1
 */
air_u32_t arm_cache_get_number_sets(air_u32_t ccsidr) {
    return ((ccsidr >> 13U) & 0x7fff) + 1;
}

/**
 * \details
 * Register Size - Associativity
 */
air_u32_t arm_cache_get_way_shift(air_u32_t associativity) {
    return __builtin_clz(associativity - 1);
}

void arm_cache_invalidate_level(air_u32_t level, air_u8_t InD) {
    air_u32_t ccsidr;
    air_u32_t line_shift;
    air_u32_t num_sets;
    air_u32_t set;
    air_u32_t way_shift;
    air_u32_t associativity;
    air_u32_t way;

    /* bitwise shift due to CSSELR level starting at bit 1 */
    ccsidr = arm_cache_get_size_id_for_level((level << 1U) | InD);

    line_shift = arm_cache_get_line_shift(ccsidr);
    num_sets = arm_cache_get_number_sets(ccsidr);
    associativity = arm_cache_get_associativity(ccsidr);
    way_shift = arm_cache_get_way_shift(associativity);

    for (set = 0; set < num_sets; set++) {
        for (way = 0; way < associativity; way++) {

            air_u32_t set_way = (way << way_shift)
                | (set << line_shift)
                | (level << 1U);

            arm_cp15_data_cache_invalidate_line_by_set_and_way(set_way);
        }
    }
}

void arm_data_cache_clean_level(air_u32_t level) {
    air_u32_t ccsidr;
    air_u32_t line_shift;
    air_u32_t num_sets;
    air_u32_t set;
    air_u32_t way_shift;
    air_u32_t associativity;
    air_u32_t way;

    /* bitwise shift due to CSSELR level starting at bit 1 */
    ccsidr = arm_cache_get_size_id_for_level((level << 1U));

    line_shift = arm_cache_get_line_shift(ccsidr);
    num_sets = arm_cache_get_number_sets(ccsidr);
    associativity = arm_cache_get_associativity(ccsidr);
    way_shift = arm_cache_get_way_shift(associativity);

    for (set = 0; set < num_sets; set++) {
        for (way = 0; way < associativity; way++) {

            air_u32_t set_way = (way << way_shift)
                | (set << line_shift)
                | (level << 1U);

            arm_cp15_data_cache_clean_line_by_set_and_way(set_way);
        }
    }
}
