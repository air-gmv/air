/**
 * @file
 * @author lumm
 * @brief BARE-C partition functions definitions
 */

#include <air.h>

typedef struct {
    air_uptr_t instr;
    air_uptr_t lr;
} pos_hm_stack;

void pos_exceptions(void);

void pos_hm_undef(pos_hm_stack *hm_stack);
void pos_hm_pref_abort(pos_hm_stack *hm_stack);
void pos_hm_data_abort(pos_hm_stack *hm_stack);
