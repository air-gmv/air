/*
 * Copyright (C) 2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file hm.h
 * \author lumm
 * \brief HM function declaration
 */

#ifndef HM_H_
#define HM_H_

#include <armv7.h>
#include <workspace.h>

/**
 * \brief ARM Health Monitor handler
 * \param id the architecture specific exception ID
 * \param fsr the Fault Status Register (IFSR, DFSR or SPSR for undef)
 * \param far the Fault Address Register (IFAR, DFAR or LR for undef)
 * \param lr the LR (prefered returned address)
 * \return POS HM handler address (or NULL if continues execution)
 */
air_uptr_t arm_hm_handler(arm_exception_e id, air_u32_t fsr, air_u32_t far, air_uptr_t lr);

static air_uptr_t arm_partition_hm_handler(air_u32_t id, pmk_core_ctrl_t *core);
static air_boolean_t arm_hm_undef_is_fpu(air_u32_t ret_addr, air_boolean_t is_T32);

/**
 * \brief Restores the FPU with the current context values
 * \param vfp_context Current context fpu registers
 */
void arm_restore_fpu(arm_vfp_context_t *vfp_context);

#endif /* HM_H_ */

