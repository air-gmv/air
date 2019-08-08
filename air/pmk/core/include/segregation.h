/*
 * Copyright (C) 2008-2015  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief Definitions, structures and functions to control the PMK memory
 *        segregation
 */

#include <pmk.h>

#ifndef __SEGREGATION_H__
#define __SEGREGATION_H__

/**
 * @defgroup pmk_segregation PMK - Segregation
 * @brief Definitions, structures and functions to control the PMK memory
 *        segregation
 * @{
 */

/**
 * @brief Align address macro
 * @param addr Address to align
 * @param align Desired alignment
 */
#define ADDR_ALIGN(addr, align)     (((addr) + ((align) - 1)) & ~((align) - 1))

/**
 * @brief Memory access permissions enumeration
 */
typedef enum {

    /** User-level read permission                              */
    PMK_MMU_R           = AIR_MEMORY_ATTRIBUTE_READ,
    /** User-level write permission                             */
    PMK_MMU_W           = AIR_MEMORY_ATTRIBUTE_WRITE,
    /** User-level executable permission                        */
    PMK_MMU_E           = AIR_MEMORY_ATTRIBUTE_EXECUTE,
    /** Supervisor-level read permission                        */
    PMK_MMU_SR          = 0x08,
    /** Supervisor-level write permission                       */
    PMK_MMU_SW          = 0x10,
    /** Supervisor-level executable permission                  */
    PMK_MMU_SE          = 0x20,
    /** MMU page is cacheable                                   */
    PMK_MMU_CACHEABLE   = 0x40,
    /** MMU page is from device                                 */
    PMK_MMU_DEVICE      = 0x80,

} pmk_mmu_flags_e;

/**
 * @brief Partition Memory map information structure
 */
typedef struct {

    void *p_addr;                    /**< Physical address                   */
    void *v_addr;                    /**< Virtual address                    */
    air_u32_t p_unit;                 /**< Page unit                          */
    air_sz_t size;                     /**< Size of the partition              */

} pmk_mmap_t;

/**
 * @brief Initializes the partition memory areas and load their executables
 *        into the main memory.
 */
void pmk_segregation_int(void);

/**
 * @brief Write a simple value into user space
 * @param ctx Executing core context
 * @param x value to copy to partition space
 * @param ptr destination address, in partition space
 * @return 0 if no faults, 1 otherwise
 */
#define pmk_segregation_put_user(ctx, x, ptr) \
        cpu_copy_to_user((ctx), (ptr), &(x), sizeof((x)))

/**
 * @brief Get a simple variable from partition space
 * @param ctx Executing core context
 * @param x Variable to copy from the partition state
 * @param ptr Source address, in partition space
 * @return 0 if no faults, 1 otherwise
 */
#define pmk_segregation_get_user(ctx, x, ptr) \
        cpu_copy_from_user((ctx), &(x), (ptr), sizeof((x)))

/**
 * @brief Secure copy to partition address
 * @param ctx Executing core context
 * @param src Source address (kernel space)
 * @param dst Destination address (partition space)
 * @param size Size of the memory block
 * @return 0 if no faults, 1 otherwise
 */
#define pmk_segregation_copy_to_user(ctx, dst, src, size) \
        cpu_copy_to_user((ctx), (dst), (src), (size))

/**
 * @brief Secure copy from partition address
 * @param ctx Executing core context
 * @param src Source address (partition space)
 * @param dst Destination address (kernel space)
 * @param size Size of the memory block
 * @return 0 if no faults, 1 otherwise
 */
#define pmk_segregation_copy_from_user(ctx, dst, src, size) \
        cpu_copy_from_user((ctx), (dst), (src), (size))

/**
 * @brief End of the kernel memory area (from link commands)
 */
extern air_u32_t air_kernel_memory_end;
/**
 * @brief Begin of the kernel memory area (from link commands)
 */
extern air_u32_t air_kernel_memory_start;

/** @} */

#endif /* __SEGREGATION_H__ */
