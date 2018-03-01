/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/bspIo.h>

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
  #if (BSP_PRINT_EXCEPTION_CONTEXT)
    if ( source == RTEMS_FATAL_SOURCE_EXCEPTION ) {
      rtems_exception_frame_print( (const rtems_exception_frame *) code );
    }
  #endif

  #if (BSP_PRESS_KEY_FOR_RESET)
    printk( "\nFATAL ERROR - Executive shutdown! Any key to reboot..." );

    /*
     * Wait for a key to be pressed
     */
    while ( getchark() == -1 )
      ;

    printk("\n");
  #endif

  /*
   *  Check both conditions -- if you want to ask for reboot, then
   *  you must have meant to reset the board.
   */
  #if (BSP_PRESS_KEY_FOR_RESET) || (BSP_RESET_BOARD_AT_EXIT)
    bsp_reset();
  #endif
}
