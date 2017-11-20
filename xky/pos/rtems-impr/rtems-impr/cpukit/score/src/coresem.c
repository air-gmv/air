/**
 *  @file
 *
 *  @brief Core Semaphore Initialize
 *  @ingroup ScoreSemaphore
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/coresemimpl.h>

void _CORE_semaphore_Initialize(
  CORE_semaphore_Control *the_semaphore,
  uint32_t                initial_value
)
{
  the_semaphore->count = initial_value;

  _Thread_queue_Object_initialize( &the_semaphore->Wait_queue );
}
