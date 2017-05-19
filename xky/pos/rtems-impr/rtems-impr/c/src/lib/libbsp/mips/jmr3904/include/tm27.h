/*
 *  tm27.h
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

/*
 *  Define the interrupt mechanism for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 1

#if 0
#define Install_tm27_vector( handler ) \
    (void) set_vector( handler, TX3904_IRQ_SOFTWARE_1, 1 ); \

#define Cause_tm27_intr() \
    asm volatile ( "syscall 0x01" : : );

#define CLOCK_VECTOR TX3904_IRQ_TMR0

#define Clear_tm27_intr() /* empty */

#define Lower_tm27_intr() /* empty */
#else
#define Install_tm27_vector( handler ) \
    (void) set_vector( handler, TX3904_IRQ_TMR0, 1 ); \

#define Cause_tm27_intr() \
  do { \
    uint32_t   _clicks = 20; \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_CCDR, 0x3 ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_CPRA, _clicks ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_TISR, 0x00 ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_ITMR, 0x8001 ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_TCR,   0xC0 ); \
    *((volatile uint32_t*) 0xFFFFC01C) = 0x00000700; \
  } while(0)

#define Clear_tm27_intr() \
  do { \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_ITMR, 0x0001 ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_CCDR, 0x3 ); \
    TX3904_TIMER_WRITE( TX3904_TIMER0_BASE, TX3904_TIMER_TISR,   0x00 ); \
  } while(0)

#define Lower_tm27_intr() \
  mips_enable_in_interrupt_mask( 0xff01 );

#endif

#endif
