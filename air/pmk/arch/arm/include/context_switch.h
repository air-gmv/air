/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file context_switch.h
 * @author lumm
 * @brief Context Switch headers
 */

#include <armv7.h>

void arm_core_context_save(void *core);
void arm_core_context_restore(void *core);
