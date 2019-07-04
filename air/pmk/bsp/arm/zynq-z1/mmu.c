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
 * @brief MMU configuration.
 */

#include <bsp.h>
#include <armcp15.h>
#include <a9mpcore_start.h>

static const arm_cp15_start_section_config
zynq_mmu_config_table[] = {
  ARMV7_CP15_START_DEFAULT_SECTIONS,
#if defined(PMK_SMP)
  {
    .begin = 0xffff0000U,
    .end = 0xffffffffU,
    .flags = ARMV7_MMU_DEVICE
  },
#endif
  {
    .begin = 0xe0000000U,
    .end = 0xe0200000U,
    .flags = ARMV7_MMU_DEVICE
  }, {
    .begin = 0xf8000000U,
    .end = 0xf9000000U,
    .flags = ARMV7_MMU_DEVICE
  }
};

/*
 * Make weak and let the user override.
 */
BSP_START_TEXT_SECTION void zynq_setup_mmu_and_cache(void) __attribute__ ((weak));

BSP_START_TEXT_SECTION void zynq_setup_mmu_and_cache(void)
{
  air_u32_t ctrl = arm_cp15_start_setup_mmu_and_cache(
    ARM_CP15_CTRL_A,
    ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_Z
  );

  arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
    ctrl,
    (air_u32_t *) bsp_translation_table_base,
    ARM_MMU_DEFAULT_CLIENT_DOMAIN,
    &zynq_mmu_config_table[0],
    RTEMS_ARRAY_SIZE(zynq_mmu_config_table)
  );
}
