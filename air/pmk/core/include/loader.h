/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 *  @file
 *  @author pfnf
 *  @brief Defines the types and functions to handle compressed partition ELFs
 */

#include <pmk.h>

#ifndef __LOADER_H__
#define __LOADER_H__

/**
 * @brief Partition ELF load-able segment information
 * @ingroup pmk_partition
 */
typedef struct {

    void *addr;                     /**< segment address            */
    air_uptr_t offset;               /**< segment data offset        */
    air_sz_t size;                    /**< segment memory size        */

} pmk_segment_t;

/**
 * @brief Partition compressed ELF information
 * @ingroup pmk_partition
 */
typedef struct {

    void *entry;                    /**< entry point            */
    air_u32_t count;                 /**< segment count          */
    pmk_segment_t *segments;        /**< program segments       */
    air_u8_t *data;                  /**< compress program       */

} pmk_elf_t;

/**
 * @brief Loads a partition elf data into the main memory
 * @param elf Compressed ELF data pointer
 * @param addr Address of the partition (beware MMU state)
 * @ingroup pmk_partition
 */
void pmk_partition_load(pmk_elf_t *elf, void *addr);

#endif /* __LOADER_H__ */
