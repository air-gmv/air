/*
 * Copyright (C) 2008-2020  GMVIS Skysoft S.A.
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

#define COLOR_KNRM "\x1B[0m\0"
#define COLOR_KRED "\x1B[31m\0"
#define COLOR_KGREEN "\x1B[32m\0"
#define COLOR_KYELLOW "\x1B[33m\0"
#define COLOR_KBLUE "\x1B[34m\0"
#define COLOR_KMAGENTA "\x1B[35m\0"
#define COLOR_KCYAN "\x1B[36m\0"
#define COLOR_KWHITE "\x1B[37m\0"
#define COLOR_RESET "\033[0m\0"

#if defined(stdin)
#else
typedef enum
{

    stdin = 0x00,
    stdout = 0x01,
    stderr = 0x02

} fake_stdFiles;
#endif
/**
 * @brief Writes the C string pointed by format to the standard output
 * @param fmt C string that contains the text to be written
 */
extern void libpprintf_internal_pprintf(const char *fmt, ...);
#define pprintf(fmt...) libpprintf_internal_pprintf(fmt)
#define printf(fmt...) libpprintf_internal_pprintf(fmt)
#define fprintf(fstd, fmt...) libpprintf_internal_pprintf(fmt)
#endif /* __PRINTF_H__ */
