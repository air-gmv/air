/**
 *  @file
 *  @brief pthread_getconcurrency() API Conformance Test
 */

/*
 *  COPYRIGHT (c) 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>

#ifndef _POSIX_THREADS
#error "rtems is supposed to have pthread_getconcurrency"
#endif

int test( void );

int test( void )
{
  int level;

  level = pthread_getconcurrency();

  return (level == 0);
}
