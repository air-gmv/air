/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */
/**
 * @file asm.h
 * @author lumm
 * @brief Assembly macros to improve readability.
 */

#ifndef ARM_ASM_H
#define ARM_ASM_H

#ifdef ASM

#define global(label) \
    .globl label; \
    label:

#endif /* ASM */

#endif /* ARM_ASM_H */
