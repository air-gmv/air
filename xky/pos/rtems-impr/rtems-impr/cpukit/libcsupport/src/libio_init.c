/**
 *  @file
 *
 *  @brief RTEMS LibIO Initialization
 *  @ingroup LibIO
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio_.h>               /* libio_.h pulls in rtems */
#include <rtems.h>
#include <rtems/assoc.h>                /* assoc.h not included by rtems.h */

#include <stdio.h>                      /* O_RDONLY, et.al. */
#include <fcntl.h>                      /* O_RDONLY, et.al. */
#include <errno.h>

#include <errno.h>
#include <string.h>                     /* strcmp */
#include <unistd.h>
#include <stdlib.h>                     /* calloc() */

#include <rtems/libio.h>                /* libio.h not pulled in by rtems */
#include <rtems/sysinit.h>

/*
 *  File descriptor Table Information
 */

rtems_id rtems_libio_semaphore;

void *rtems_libio_iop_free_head;

void **rtems_libio_iop_free_tail = &rtems_libio_iop_free_head;

static void rtems_libio_init( void )
{
    rtems_status_code rc;
    uint32_t i;
    rtems_libio_t *iop;
    int eno;

    if (rtems_libio_number_iops > 0)
    {
        iop = rtems_libio_iop_free_head = &rtems_libio_iops[0];
        for (i = 0 ; (i + 1) < rtems_libio_number_iops ; i++, iop++)
          iop->data1 = iop + 1;
        iop->data1 = NULL;
        rtems_libio_iop_free_tail = &iop->data1;
    }

  /*
   *  Create the posix key for user environment.
   */
  eno = pthread_key_create(
    &rtems_current_user_env_key,
    rtems_libio_free_user_env
  );
  if (eno != 0) {
    _Internal_error( INTERNAL_ERROR_LIBIO_USER_ENV_KEY_CREATE_FAILED );
  }

  /*
   *  Create the binary semaphore used to provide mutual exclusion
   *  on the IOP Table.
   */

  rc = rtems_semaphore_create(
    RTEMS_LIBIO_SEM,
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
    RTEMS_NO_PRIORITY,
    &rtems_libio_semaphore
  );
  if ( rc != RTEMS_SUCCESSFUL ) {
    _Internal_error( INTERNAL_ERROR_LIBIO_SEM_CREATE_FAILED );
  }
}

RTEMS_SYSINIT_ITEM(
  rtems_libio_init,
  RTEMS_SYSINIT_LIBIO,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

RTEMS_SYSINIT_ITEM(
  rtems_libio_post_driver,
  RTEMS_SYSINIT_STD_FILE_DESCRIPTORS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
