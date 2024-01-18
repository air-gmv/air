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
 * \param frame interrupt stack frame
 * \param core pmk core control structure
 * \return POS HM handler address (or NULL if continues execution)
 */
air_uptr_t *arm_hm_handler(arm_interrupt_stack_frame_t *frame, pmk_core_ctrl_t *core);

air_uptr_t *arm_partition_hm_handler(air_u32_t id, pmk_core_ctrl_t *core);

/**
 * \brief Restores the FPU with the current context values
 * \param vfp_context Current context fpu registers
 */
//void arm_restore_fpu(arm_vfp_context_t *vfp_context);
void arm_restore_fpu(arm_interrupt_stack_frame_t *frame);
#endif /* HM_H_ */

