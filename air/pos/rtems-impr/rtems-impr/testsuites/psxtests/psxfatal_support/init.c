/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "PSXFATAL " FATAL_ERROR_TEST_NAME;

static void print_test_begin_message(void)
{
  static bool done = false;

  if (!done) {
    done = true;
    TEST_BEGIN();
  }
}

void *POSIX_Init(
  void *argument
)
{
  print_test_begin_message();
  force_error();
  printk( "Fatal error (%s) NOT hit\n", FATAL_ERROR_DESCRIPTION );
  rtems_test_exit(0);
}

void Put_Error( rtems_fatal_source source, rtems_fatal_code error )
{
  if ( source == INTERNAL_ERROR_CORE ) {
    printk( rtems_internal_error_text( error ) );
  }
  else if (source == INTERNAL_ERROR_RTEMS_API ){
    printk( "%s", rtems_status_text( error ) );
  }
  else if (source == INTERNAL_ERROR_POSIX_API ) {
    printk(
      "SOURCE=%d ERROR=%" PRIuMAX " %s",
      source,
      (uintmax_t) error,
      strerror( (int) error )
    );
  }
}

void Put_Source( rtems_fatal_source source )
{
  printk( "%s", rtems_fatal_source_text( source ) );
}

void Fatal_extension(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   error
)
{
  print_test_begin_message();
  printk( "Fatal error (%s) hit\n", FATAL_ERROR_DESCRIPTION );

  if ( source != FATAL_ERROR_EXPECTED_SOURCE ){
    printk( "ERROR==> Fatal Extension source Expected (");
    Put_Source( FATAL_ERROR_EXPECTED_SOURCE );
    printk( ") received (");
    Put_Source( source );
    printk( ")\n" );
  }

  if ( always_set_to_false )
    printk( "ERROR==> Fatal Extension is internal set to true expected false\n" );

  if ( error !=  FATAL_ERROR_EXPECTED_ERROR ) {
    printk( "ERROR==> Fatal Error Expected (");
    Put_Error( source, FATAL_ERROR_EXPECTED_ERROR );
    printk( ") received (");
    Put_Error( source, error );
    printk( ")\n" );
  }

  if (
    source == FATAL_ERROR_EXPECTED_SOURCE
      && !always_set_to_false
      && error == FATAL_ERROR_EXPECTED_ERROR
  ) {
    TEST_END();
  }
}
