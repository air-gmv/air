/*
 * Copyright (C) 2018  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * https://spass-git-ext.gmv.com/AIR/AIR/raw/master/air/LICENSE
 */
/**
 * \file bsp_console.h
 * \author lumm
 * \brief BSP console
 */

#ifndef BSP_CONSOLE_H_
#define BSP_CONSOLE_H_

#include <a9mpcore.h>

void arm_uart_transmit(char ch);

#define bsp_console_outbyte(ch) arm_uart_transmit(ch)

#endif /* BSP_CONSOLE_H_ */
