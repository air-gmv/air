/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file bsp.h
 * @author lumm
 * @brief BSP headers and inline assembly functions.
 */

#ifndef ASM

#include <air_arch.h>
#include <armv7.h>

#define SCU_BASE_MEMORY 0xf8f00000
#define GT_BASE_MEMORY  0xf8f00200

void bsp_start_hook(void);

static inline clear_bss() {
    memset(bsp_section_bss_start, 0, bsp_section_bss_size);
//  air_i8_t *mem = (air_i8_t *)bsp_section_bss_start;
//  for(air_u32_t i = 0; i < bsp_section_bss_size; i++) {
//
//      *mem = 0;
//      mem++;
//  }
}

static inline copy_vector_table() {
    memcpy(bsp_vector_table_begin, vector_table_begin, 8);
//  if (vector_table_begin != bsp_vector_table_begin) {
//      air_i8_t *dst = (air_i8_t *) bsp_vector_table_begin;
//      air_i8_t *src = (air_i8_t *) vector_table_begin;
//
//      for(air_u32_t i = 0; i < 8; i++) {
//          *dst++ = *src++;
//      }
//  }
}

#endif /* ASM */
