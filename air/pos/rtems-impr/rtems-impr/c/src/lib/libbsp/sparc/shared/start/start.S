/**
 *  Common start code for SPARC.
 *
 *  This is based on the file srt0.s provided with the binary
 *  distribution of the SPARC Instruction Simulator (SIS) found
 *  at ftp://ftp.estec.esa.nl/pub/ws/wsd/erc32.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/asm.h>
#include <rtems/score/percpu.h>
#include <bspopts.h>
#include <air.h>

/*
 *  @brief AIR removes trap table definition to impose its definition
           Maintained the start.S used in previous versions
 */

#if defined(RTEMS_SMP) && defined(BSP_LEON3_SMP)
  #define START_LEON3_ENABLE_SMP
#endif


  .section    ".text"
  PUBLIC(start)
  .global start, __bsp_mem_init

SYM(start):
#if SYM(start) != start
start:
#endif 
    .global SYM(_ERC32_MEC_Timer_Control_Mirror), SYM(rdb_start), SYM(CLOCK_SPEED)

SYM(_ERC32_MEC_Timer_Control_Mirror):


/*
 *  This is the hard reset code.
 */

        PUBLIC(hard_reset)
SYM(hard_reset):

    sethi   %hi(_Per_CPU_Information), %g6 ! get per-CPU control
    add %g6, %lo(_Per_CPU_Information), %g6

 #if defined(START_LEON3_ENABLE_SMP)
    rd  %asr17, %o0     ! get CPU identifier
    srl %o0, LEON3_ASR17_PROCESSOR_INDEX_SHIFT, %o0

    sll %o0, PER_CPU_CONTROL_SIZE_LOG2, %l0
    add %g6, %l0, %g6

    /* If LEON3_Boot_Cpu < 0 then assign us as boot CPU and continue. */
    set SYM(LEON3_Boot_Cpu), %o1
    ld  [%o1], %o2
    tst %o2
    bneg    .Lbootcpu
     nop

    ld  [%g6 + PER_CPU_INTERRUPT_STACK_HIGH], %sp ! set stack pointer
    sub %sp, 4, %sp     ! stack starts at end of area - 4
    andn    %sp, 0x0f, %sp      ! align stack on 16-byte boundary
    mov %sp, %fp        ! set frame pointer

    call    SYM(bsp_start_on_secondary_processor) ! does not return
     sub    %sp, SPARC_MINIMUM_STACK_FRAME_SIZE, %sp
.Lbootcpu:
    st  %o0, [%o1]
#endif

    set (SYM(rdb_start)), %g5   ! End of RAM
    st  %sp, [%g5]
    sub %sp, 4, %sp     ! stack starts at end of RAM - 4
    andn    %sp, 0x0f, %sp      ! align stack on 16-byte boundary
        mov     %sp, %fp                ! Set frame pointer
        nop

        /*
         *  Copy the initialized data to RAM
         *
         *  FROM:   _data_load_start
         *  TO:     _data_start
         *  LENGTH: (__bss_start - _data_start) bytes
         */

        sethi %hi(_data_load_start),%g1 ! g1 = start of initialized data in ROM
        or    %g1,%lo(_data_load_start),%g1

        sethi %hi(_data_start),%g3      ! g3 = start of initialized data in RAM
        or    %g3,%lo(_data_start),%g3

        sethi %hi(__bss_start), %g2     ! g2 = end of initialized data in RAM
        or    %g2,%lo(__bss_start),%g2

    cmp   %g1, %g3
    be    1f
    nop

copy_data:
        ldd   [%g1], %g4
        std   %g4 , [%g3]               ! copy this double word
        add   %g3, 8, %g3               ! bump the destination pointer
        add   %g1, 8, %g1               ! bump the source pointer
        cmp   %g3, %g2                  ! Is the pointer past the end of dest?
        bl    copy_data
        nop
 
  /**
   * clear the bss
   **/
1:

  sethi   %hi(_edata),%g2
  or      %g2,%lo(_edata),%g2                 ! g2 = start of bss
  sethi   %hi(_end),%g3
  or      %g3,%lo(_end),%g3                   ! g3 = end of bss
  mov     %g0,%g1                             ! so std has two zeros

zerobss:
  std     %g0,[%g2]
  add     %g2,8,%g2
  cmp     %g2,%g3
  bleu,a  zerobss
  nop

  mov     %0, %o2                             ! environ
  mov     %0, %o1                             ! argv
  mov     %0, %o0                             ! argc
  call    SYM(boot_card)
  sub     %sp, 0x60, %sp                      ! room for boot_card to save args

  /**
   * Set partition to idle
  */
        PUBLIC(bsp_cleanup)
SYM(bsp_cleanup):
  set     AIR_SYSCALL_SET_PARTITION_MODE, %o5 ! set partition mode trap id
  set     AIR_MODE_IDLE, %o1                  ! mode idle
  set     0xFFFFFFFF, %o0                     ! current partition
  ta      AIR_SYSCALL_OS_TRAP                 ! call AIR OS trap

  .global SYM(rdb_start)                      ! place to store the end of systen                             ! memory
SYM(rdb_start):
  nop
/* end of file */

