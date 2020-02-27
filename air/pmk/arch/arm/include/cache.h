/*
 * Copyright (C) 2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file cache.h
 * \author lumm
 * \brief Cache maintenance definitions
 */

#ifndef CACHE_H_
#define CACHE_H_

#include <armv7.h>

/**
 * \brief Invalidates all instruction cache entries
 */
void arm_instruction_cache_invalidate(void);

/**
 * \brief Invalidates all data cache entries
 */
void arm_data_cache_invalidate(void);

/**
 * \brief Cleans all data cache entries
 */
void arm_data_cache_clean(void);

/**
 * \brief Invalidates all TLB entries
 */
void arm_tlb_invalidate(void);

/**
 * \brief Invalidates all BP entries
 */
void arm_branch_predictor_invalidate(void);

/**
 * \brief Invalidates instruction or data cache by level
 * \param level level to invalidate
 * \param InD CSSELR InD bit to select between instruction or data cache
 */
void arm_cache_invalidate_level(air_u32_t level, air_u8_t InD);

/**
 * \brief Cleans data cache by level
 * \param level level to clean
 */
void arm_data_cache_clean_level(air_u32_t level);

air_u32_t arm_cache_get_size_id_for_level(air_u32_t level);
air_u32_t arm_cache_get_line_shift(air_u32_t ccsidr);
air_u32_t arm_cache_get_number_sets(air_u32_t ccsidr);
air_u32_t arm_cache_get_associativity(air_u32_t ccsidr);
air_u32_t arm_cache_get_way_shift(air_u32_t associativity);

#define CP15_CSSELR_DATA 0
#define CP15_CSSELR_INST 1

#endif /* CACHE_H_ */
