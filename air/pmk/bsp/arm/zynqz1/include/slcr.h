/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file slcr.h
 * @author lumm
 * @brief System Level Control Registers Definitions
 */

#ifndef ARM_ZYNQ_Z1_SLCR_H
#define ARM_ZYNQ_Z1_SLCR_H

#define SLCR_BASE_MEMORY                0xf8000000

/* too many registers. defined in place of use */

#define UART_CLK_CTRL_CLKACT0_ENABLE    (1U << 0)
#define UART_CLK_CTRL_CLKACT1_ENABLE    (1U << 1)
#define UART_CLK_CTRL_SRCSEL(mask)      (mask << 4)
#define UART_CLK_CTRL_DIVISOR(val)      (val << 8)

#define MIO_PIN_SLOW_CMOS_EDGE          0x0
#define MIO_PIN_FAST_CMOS_EDGE          0x1

#define MIO_PIN_IO_LVCMOS18             (0x1)
#define MIO_PIN_IO_LVCMOS25             (0x2)
#define MIO_PIN_IO_LVCMOS33             (0x3)
#define MIO_PIN_IO_HSTL                 (0x4)

#define MIO_PIN_TRI_ENABLE              (1U << 0)
#define MIO_PIN_L0_SEL(mask)            (mask << 1)
#define MIO_PIN_L1_SEL(mask)            (mask << 2)
#define MIO_PIN_L2_SEL(mask)            (mask << 3)
#define MIO_PIN_L3_SEL(mask)            (mask << 5)
#define MIO_PIN_SPEED(val)              (val << 8)
#define MIO_PIN_IO_TYPE(val)            (val << 9)
#define MIO_PIN_PULLUP(val)             (val << 12)
#define MIO_PIN_DISABLE_RCVR            (1U << 13)

#endif /* ARM_ZYNQ_Z1_SLCR_H */
