/**
 * @file
 *
 * @brief Function Unlocks a Spin Lock Object
 * @ingroup POSIXAPI
 */

/*
 *  POSIX Spinlock Manager -- Wait at a Spinlock
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2016 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/spinlockimpl.h>

int pthread_spin_unlock( pthread_spinlock_t *lock )
{
  POSIX_Spinlock_Control *the_spinlock;
  ISR_Level               level;

  the_spinlock = _POSIX_Spinlock_Get( lock );
  level = the_spinlock->interrupt_state;
#if defined(RTEMS_SMP)
  _SMP_ticket_lock_Release(
    &the_spinlock->Lock,
    &_Per_CPU_Get()->Lock_stats_context
  );
#endif
  _ISR_Local_enable( level );
  return 0;
}
