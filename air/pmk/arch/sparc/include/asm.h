/*
 * Copyright (C) 2013-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief Contains ASM macros to improve assembly readability
 **/
 
#ifndef __ASM_H__
#define __ASM_H__

#include <sparc.h>
#include <asm_offsets.h>

#ifdef ASM

/** @brief Macro for symbol definitions                                     */
#define SYM(x)                      x

/** @brief Macro for register definitions                                   */
#define REG(x)                      x

/** @brief Macro for global symbols definitions                             */
#define GLOBAL(x)                   .align 0x4; .globl SYM(x)

/** @brief Reset trap handler entry                                         */
#define TRAP_RESET(n, handler)      \
    sethi   %hi(handler), %l4;      \
    jmp     %l4 + %lo(handler);     \
    mov     n, %l3;                 \
    nop;

/** @brief Default trap handler entry                                       */
#define TRAP_HANDLER(n, handler)    \
    mov     %psr, %l0;              \
    sethi   %hi(handler), %l4;      \
    jmp     %l4 + %lo(handler);     \
    mov     n, %l3;

/**  @brief Default trap ignore entry                                       */
#define TRAP_IGNORE()               \
    jmpl    %l1, %g0;               \
    rett    %l2;                    \
    nop;                            \
    nop;

#define TRAP_RESUME()               \
    jmpl    %l2;                    \
    rett    %l2 + 0x4;              \
    nop;                            \
    nop;

#endif

#endif /* __ASM_H__ */
