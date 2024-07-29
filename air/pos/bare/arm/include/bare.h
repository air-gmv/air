/**
 * @file
 * @author lumm
 * @brief BARE-C partition functions definitions
 */

#ifndef ARM_POS_BARE_H
#define ARM_POS_BARE_H

#ifndef ASM

#include <air.h>

typedef struct {
    air_uptr_t *instr;
    air_uptr_t *lr;
} pos_hm_stack;

void pos_hm_undef(pos_hm_stack *hm_stack);
void pos_hm_pref_abort(pos_hm_stack *hm_stack);
void pos_hm_data_abort(pos_hm_stack *hm_stack);

/**
 * @brief Install ISR handler (per core)
 * @param tn ISR number
 * @param new New ISR handler
 * @param[out] old Old ISR handler
 */
void isr_install_handler(air_u32_t tn, void *new, void **old);

void arm_pos_smp_init(void);

/**
 * @brief Wake core after
 * @param ticks Number of ticks of sleep
 */
void wake_after(air_clocktick_t ticks);
void delay(float delay_seconds); 

#endif /* ASM */
#endif /* ARM_POS_BARE_H */
