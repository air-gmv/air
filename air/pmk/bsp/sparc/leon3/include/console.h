/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief Console related functions
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <pmk.h>

/**
 * @defgroup bsp_leon_console BSP - LEON 3 & 4 Console
 * @brief Board Support package console support functions
 * @{
 */

/**
 * @brief Console Initialization
 * @return 0 if the device was initialize successfully, 1 otherwise
 */
int console_init(void);

/**
 * @brief Send a byte to the console
 * @param ch byte to send
 */
void console_outbyte(char ch);

/** @} */

#endif /* __CONSOLE_H__ */
