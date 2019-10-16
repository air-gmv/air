/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file segregation.c
 * \author lumm
 * \brief MMU configuration
 */

#include <bsp_segregation.h>
#include <cpu.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

extern air_u32_t air_workspace;
extern air_u32_t air_kernel_memory_start;
extern air_u32_t air_kernel_memory_end;
extern air_u32_t air_kernel_end;
/**
 * \brief mmu setup for each partition containing pmk
 */
void bsp_segregation(pmk_partition_t *partition) {

#ifdef PMK_DEBUG
    printk("   :: BSP segregation\n"
            "      air_kernel_memory_start = 0x%08x\n"
            "      air_kernel_memory_end   = 0x%08x\n"
            "      kernel size (end-start) = 0x%08x\n"
            "      air_workspace           = 0x%08x\n",
            (air_uptr_t)&air_kernel_memory_start,
            (air_uptr_t)&air_kernel_memory_end,
            (air_u32_t)&air_kernel_memory_end - (air_u32_t)&air_kernel_memory_start,
            (air_uptr_t)&air_workspace);
#endif

    /* Map PMK into partition mmu with supervisor access */
    cpu_segregation_map_memory(
            partition->mmu_ctrl,
            (void *)&air_kernel_memory_start,
            (void *)&air_kernel_memory_start,
            (air_sz_t)&air_kernel_memory_end - (air_sz_t)&air_kernel_memory_start,
            0x00100000,
            PMK_MMU_SR | PMK_MMU_SW | PMK_MMU_SE | PMK_MMU_CACHEABLE);

    /* TODO SECURITY FLAW ALSO IN SPARC */
    cpu_segregation_map_memory(
            partition->mmu_ctrl,
            (void *)0xe0000000,
            (void *)0xe0000000,
            (air_sz_t)(0xffffffff - 0xe0000000 + 1),
            0x00100000,
            PMK_MMU_SR | PMK_MMU_SW | PMK_MMU_SE | PMK_MMU_DEVICE);
}
