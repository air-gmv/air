/**
 * @file
 *
 * @ingroup lpc32xx_mmu
 *
 * @brief MMU API.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC32XX_MMU_H
#define LIBBSP_ARM_LPC32XX_MMU_H

#include <libcpu/arm-cp15.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup lpc32xx_mmu MMU Support
 *
 * @ingroup lpc32xx
 *
 * @brief MMU support.
 *
 * @{
 */

#define LPC32XX_MMU_CLIENT_DOMAIN 15U

#define LPC32XX_MMU_READ_ONLY \
  ((LPC32XX_MMU_CLIENT_DOMAIN << ARM_MMU_SECT_DOMAIN_SHIFT) \
    | ARM_MMU_SECT_DEFAULT)

#define LPC32XX_MMU_READ_ONLY_CACHED \
  (LPC32XX_MMU_READ_ONLY | ARM_MMU_SECT_C | ARM_MMU_SECT_B)

#define LPC32XX_MMU_READ_WRITE \
  ((LPC32XX_MMU_CLIENT_DOMAIN << ARM_MMU_SECT_DOMAIN_SHIFT) \
    | ARM_MMU_SECT_AP_0 \
    | ARM_MMU_SECT_DEFAULT)

#define LPC32XX_MMU_READ_WRITE_CACHED \
  (LPC32XX_MMU_READ_WRITE | ARM_MMU_SECT_C | ARM_MMU_SECT_B)

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC32XX_MMU_H */
