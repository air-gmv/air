/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file isr.h
 * \author lumm
 * \brief Interrupt Service Routine definitions
 */

#ifndef ISR_H_
#define ISR_H_

#include <armv7.h>
#include <workspace.h>

void arm_isr_table_init(void);
air_uptr_t *arm_isr_handler(arm_interrupt_stack_frame_t *frame, pmk_core_ctrl_t *core);
air_uptr_t *arm_partition_isr_handler(air_u32_t id, pmk_core_ctrl_t *core);
air_uptr_t *arm_isr_install_handler(air_u32_t vector, void *handler);

air_uptr_t *arm_exception_handler(arm_interrupt_stack_frame_t *frame, pmk_core_ctrl_t *core);

#endif /* ISR_H_ */
