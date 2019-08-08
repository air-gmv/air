/*
 * Copyright (C) 2011  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief Simplified partition printf
 */

#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stdarg.h>

/**
 * @brief Writes the C string pointed by format to the standard output
 * @param fmt C string that contains the text to be written
 */
#ifdef DISABLE_PPRINTF
#define pprintf(fmt ...)
#else
extern void libpprintf_internal_pprintf(const char *fmt, ...);
#define pprintf(fmt ...)            libpprintf_internal_pprintf(fmt)
#endif

#endif /* __PRINTF_H__ */
