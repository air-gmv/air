/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file mmu.c
 * @author lumm
 * @brief MMU configuration
 */

#include <bsp_segregation.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

extern air_u32_t air_workspace;
extern air_u32_t air_kernel_memory_start;
extern air_u32_t air_kernel_memory_end;
extern air_u32_t air_kernel_end;
/**
 * @brief mmu setup for each partition containing pmk
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
            &air_kernel_memory_start,
            &air_kernel_memory_start,
            (air_u32_t)&air_kernel_memory_end - (air_u32_t)&air_kernel_memory_start,
            0x00100000,
            PMK_MMU_SR | PMK_MMU_SW | PMK_MMU_SE | PMK_MMU_CACHEABLE);

    /* TODO SECURITY FLAW ALSO IN SPARC */
    cpu_segregation_map_memory(
            partition->mmu_ctrl,
            (void *)0xe0000000,
            (void *)0xe0000000,
            0xffffffff - 0xe0000000 + 1,
            0x00100000,
            PMK_MMU_SR | PMK_MMU_SW | PMK_MMU_SE | PMK_MMU_DEVICE);
}


// typedef struct {
//     air_u32_t begin;
//     air_u32_t end;
//     air_u32_t flags;
// } arm_cp15_start_section_config;
//
// static const arm_cp15_start_section_config
// zynq_mmu_config_table[] = {
// #if defined(PMK_SMP)
//   {
//     .begin = 0xffff0000U,
//     .end = 0xffffffffU,
//     .flags = ARMV7_MMU_DEVICE
//   },
// #endif
//   {
//     .begin = 0xe0000000U,
//     .end = 0xe0200000U,
//     .flags = ARMV7_MMU_DEVICE
//   }, {
//     .begin = 0xf8000000U,
//     .end = 0xf9000000U,
//     .flags = ARMV7_MMU_DEVICE
//   }
// };

// /*
//  * Make weak and let the user override.
//  */
// void zynq_setup_mmu_and_cache(void) __attribute__ ((weak));
//
// void zynq_setup_mmu_and_cache(void) {
//     air_u32_t ctrl = arm_cp15_start_setup_mmu_and_cache(
//         ARM_CP15_CTRL_A,
//         ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z
//     );
//
//     arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
//         ctrl,
//         (air_u32_t *) bsp_translation_table_base,
//         ARM_MMU_DEFAULT_CLIENT_DOMAIN,
//         &zynq_mmu_config_table[0],
//         RTEMS_ARRAY_SIZE(zynq_mmu_config_table)
//     );
// }
