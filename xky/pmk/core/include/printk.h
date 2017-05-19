/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
/**
 *  @file printk.h
 *  @author pfnf
 *  @brief printk definition
 */

#include <stdarg.h>
 
#ifndef __PRINTK_H__
#define __PRINTK_H__

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

#endif /* __PRINTK_H__ */
