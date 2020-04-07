/*
 * Copyright (C) 2018  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file bsp_console.h
 * \author lumm
 * \brief BSP console
 */

#ifndef BSP_CONSOLE_H_
#define BSP_CONSOLE_H_
#if DEBUG_MONITOR != 2
#include <a9mpcore.h>

void arm_uart_transmit(char ch);

#define bsp_console_outbyte(ch) arm_uart_transmit(ch)
#endif /* DEBUG_MONITOR != 2	 */
#endif /* BSP_CONSOLE_H_ */
