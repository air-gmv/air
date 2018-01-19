/**
 * @file
 * @author pfnf
 * @brief BARE-C partition functions definitions
 */

#ifndef __BARE_H__
#define __BARE_H__

#include <air.h>

extern void sparc_lock_init(air_u32_t *lock);
extern void sparc_lock_acquire(air_u32_t *lock);
extern int sparc_lock_try_acquire(air_u32_t *lock);
extern void sparc_lock_release(air_u32_t *lock);

/**
 * @brief Lock type
 */
typedef air_u32_t smp_lock_t;

/**
 * @brief Boot core
 * @param core_id Id of the core to boot
 * @param entry_point Entry point of the core execution
 * @return INVALID_CONDIG   - if invalid core Id
 *         NO_ERROR         - otherwise
 */
air_status_code_e boot_core(air_u32_t core_id, void *entry_point);

/**
 * @brief Initialize the lock
 * @param lock lock object
 */
#define smp_lock_init(lock)         sparc_lock_init(&(lock))
/**
 * @brief Acquires the lock, if the lock isn't available, the current core
 *        busy-waits until it becomes available
 * @param lock lock object
 */
#define smp_lock_acquire(lock)      sparc_lock_acquire(&(lock))
/**
 * @brief Try to acquire the lock, if the lock isn't available, this function
 *        returns immediately
 * @param lock lock object
 * @returns true if the lock was acquired, false otherwise
 */
#define smp_lock_try_acquire(lock)  sparc_lock_try_acquire(&(lock))
/**
 * @brief Releases the lock allowing other cores to acquire it
 * @param lock lock object
 */
#define smp_lock_release(lock)      sparc_lock_release(&(lock))

/**
 * @brief Install raw ISR handler
 * @param tn ISR number
 * @param new New ISR handler
 * @param[out] old Old ISR handler
 */
void isr_install_raw_handler(air_u32_t tn, void *new, void **old);
/**
 * @brief Install ISR handler (per core)
 * @param tn ISR number
 * @param new New ISR handler
 * @param[out] old Old ISR handler
 */
void isr_install_handler(air_u32_t tn, void *new, void **old);
/**
 * @brief Wake core after
 * @param ticks Number of ticks of sleep
 */
void wake_after(air_clocktick_t ticks);

#endif  /* __BARE_H__ */
