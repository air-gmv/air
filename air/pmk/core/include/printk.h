/*
 * Copyright (C) 2008-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */
/**
 *  @file printk.h
 *  @author pfnf
 *  @brief printk definition
 */


#include <stdarg.h>
 
#ifndef __PRINTK_H__
#define __PRINTK_H__
#if DEBUG_MONITOR != 2
/**
 * @brief Writes the C string pointed by format to the standard output
 * @param fmt C string that contains the text to be written
 */
void printk(const char *fmt, ...);
/**
 * @brief Writes an character to the console
 * @param ch Character to be written
 */
void pmk_console_outbyte(char ch);

#else
#define printk //disabled
#endif /* DEBUG_MONITOR != 2 */
#endif /* __PRINTK_H__ */

