/**
 *  @file
 *  cpu_asm.S
 *
 *  @brief basic algorithms for all assembly code used in an specific CPU port
 *  of RTEMS.  These algorithms must be implemented in assembly language.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  4432    | 21/09/2009  | mcoutinho    | IPR 751
 *  5166    | 29/10/2009  | mcoutinho    | IPR 828
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8700    | 18/08/2010  | mcoutinho    | IPR 451
 *  9872    | 16/03/2011  | aconstantino | SPR 2819
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2846
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup SUPER_CORE_SPARC_CPU_SUPPORT Super Core SPARC CPU Support
 *  @{
 */

#include <rtems/asm.h>
#include <xky.h>              /* PMK paravirtualization definitions */

/**
 * \cond
 */

#if (SPARC_HAS_FPU == 1)

        .align 4
        PUBLIC(_CPU_Context_save_fp)
SYM(_CPU_Context_save_fp):

  save    %sp, -CPU_MINIMUM_STACK_FRAME_SIZE, %sp

  /**
   *  The following enables the floating point unit.
   **/
  set       XKY_SYSCALL_SPARC_ENABLE_FPU, %o5
  ta        XKY_SYSCALL_SPARC_TRAP


  ld        [%i0], %l0                          ! move argument to %l0

  /**
   * save the floating point registers to pointed by %l0
   */
  std       %f0, [%l0 + FO_F1_OFFSET]           ! save f0  and f1
  std       %f2, [%l0 + F2_F3_OFFSET]           ! save f2  and f3
  std       %f4, [%l0 + F4_F5_OFFSET]           ! save f4  and f5
  std       %f6, [%l0 + F6_F7_OFFSET]           ! save f6  and f7
  std       %f8, [%l0 + F8_F9_OFFSET]           ! save f8  and f9
  std       %f10, [%l0 + F1O_F11_OFFSET]        ! save f10 and f11
  std       %f12, [%l0 + F12_F13_OFFSET]        ! save f12 and f13
  std       %f14, [%l0 + F14_F15_OFFSET]        ! save f14 and f15
  std       %f16, [%l0 + F16_F17_OFFSET]        ! save f16 and f17
  std       %f18, [%l0 + F18_F19_OFFSET]        ! save f18 and f19
  std       %f20, [%l0 + F2O_F21_OFFSET]        ! save f20 and f21
  std       %f22, [%l0 + F22_F23_OFFSET]        ! save f22 and f23
  std       %f24, [%l0 + F24_F25_OFFSET]        ! save f24 and f25
  std       %f26, [%l0 + F26_F27_OFFSET]        ! save f26 and f27
  std       %f28, [%l0 + F28_F29_OFFSET]        ! save f28 and f29
  std       %f30, [%l0 + F3O_F31_OFFSET]        ! save f30 and f31

  /**
   * save FPU FSR register to pointed by %l0
   **/
  st        %fsr, [%l0 + FSR_OFFSET]

  ret                                           ! return
  restore                                       ! restore stack



        .align 4
        PUBLIC(_CPU_Context_restore_fp)
SYM(_CPU_Context_restore_fp):

  save      %sp, -CPU_MINIMUM_STACK_FRAME_SIZE , %sp  ! save stack

  /**
   *  The following enables the floating point unit.
   **/
  set       XKY_SYSCALL_SPARC_ENABLE_FPU, %o5
  ta        XKY_SYSCALL_SPARC_TRAP

  ld        [%i0], %l0                          ! move argument to %l0

  /**
   * restore the floating point registers from pointed by %l0
   */
  ldd       [%l0 + FO_F1_OFFSET], %f0           ! restore f0  and f1
  ldd       [%l0 + F2_F3_OFFSET], %f2           ! restore f2  and f3
  ldd       [%l0 + F4_F5_OFFSET], %f4           ! restore f4  and f5
  ldd       [%l0 + F6_F7_OFFSET], %f6           ! restore f5  and f7
  ldd       [%l0 + F8_F9_OFFSET], %f8           ! restore f8  and f9
  ldd       [%l0 + F1O_F11_OFFSET], %f10        ! restore f10  and f11
  ldd       [%l0 + F12_F13_OFFSET], %f12        ! restore f12  and f13
  ldd       [%l0 + F14_F15_OFFSET], %f14        ! restore f14  and f15
  ldd       [%l0 + F16_F17_OFFSET], %f16        ! restore f16  and f17
  ldd       [%l0 + F18_F19_OFFSET], %f18        ! restore f18  and f19
  ldd       [%l0 + F2O_F21_OFFSET], %f20        ! restore f20  and f21
  ldd       [%l0 + F22_F23_OFFSET], %f22        ! restore f22  and f23
  ldd       [%l0 + F24_F25_OFFSET], %f24        ! restore f24  and f25
  ldd       [%l0 + F26_F27_OFFSET], %f26        ! restore f26  and f27
  ldd       [%l0 + F28_F29_OFFSET], %f28        ! restore f28  and f29
  ldd       [%l0 + F3O_F31_OFFSET], %f30        ! restore f30  and f31

  /**
   * restore FPU FSR register from pointed by %l0
   **/
  ld        [%l0 + FSR_OFFSET], %fsr

  ret                                           ! return
  restore                                       ! restore stack

#endif /* SPARC_HAS_FPU */



        .align 4
        PUBLIC(_CPU_Context_switch)
SYM(_CPU_Context_switch):
  ! save global registers (skip g0 which is always 0)
  st        %g1, [%o0 + G1_OFFSET]        ! save %g1
  std       %g2, [%o0 + G2_OFFSET]        ! save %g2 and %g3
  std       %g4, [%o0 + G4_OFFSET]        ! save %g4 and %g5
  std       %g6, [%o0 + G6_OFFSET]        ! save %g6 and %g7

  ! load the address of the ISR stack nesting prevention flag
  sethi     %hi(SYM(_CPU_ISR_Dispatch_disable)), %g2
  ld        [%g2 + %lo(SYM(_CPU_ISR_Dispatch_disable))], %g2

  ! save it a bit later so we do not waste a couple of cycles
  std       %l0, [%o0 + L0_OFFSET]        ! save l0 and l1
  std       %l2, [%o0 + L2_OFFSET]        ! save l2 and l3
  std       %l4, [%o0 + L4_OFFSET]        ! save l4 and l5
  std       %l6, [%o0 + L6_OFFSET]        ! save l6 and l7

  ! Now actually save ISR stack nesting prevention flag
  st        %g2, [%o0 + ISR_DISPATCH_DISABLE_STACK_OFFSET]

  std       %i0, [%o0 + I0_OFFSET]        ! save l0 and l1
  std       %i2, [%o0 + I2_OFFSET]        ! save l2 and l3
  std       %i4, [%o0 + I4_OFFSET]        ! save l4 and l5
  std       %i6, [%o0 + I6_FP_OFFSET]     ! save l6 and l7

  std       %o0, [%o0 + O0_OFFSET]        ! save o0 and o1
  std       %o2, [%o0 + O2_OFFSET]        ! save o2 and o3
  std       %o4, [%o0 + O4_OFFSET]        ! save o4 and o5
  std       %o6, [%o0 + O6_SP_OFFSET]     ! save o6 and o7

  mov       %o0, %g1
  set       XKY_SYSCALL_SPARC_GET_PSR, %o5
  ta        XKY_SYSCALL_SPARC_TRAP
  st        %o0, [%g1 + PSR_OFFSET]       ! save PSR register

  /**
   *   _CPU_Context_restore function has the following inputs:
   *    o1 = context to restore
   *    o2 = psr
   **/
        PUBLIC(_CPU_Context_restore_heir)
SYM(_CPU_Context_restore_heir):
  /**
   *  Flush all windows with valid contents
   **/
  ta        XKY_SYSCALL_FLUSH_TRAP        ! flush all windows

  ld        [%o1 + PSR_OFFSET], %o0       ! o0 = saved psr
  set       XKY_SYSCALL_SPARC_SET_PSR, %o5
  ta        XKY_SYSCALL_SPARC_TRAP

  ! skip g0
  ld        [%o1 + G1_OFFSET], %g1        ! restore the global registers
  ldd       [%o1 + G2_OFFSET], %g2
  ldd       [%o1 + G4_OFFSET], %g4
  ldd       [%o1 + G6_OFFSET], %g6

  ! Load thread specific ISR dispatch prevention flag
  ld        [%o1 + ISR_DISPATCH_DISABLE_STACK_OFFSET], %o2
  sethi     %hi(SYM(_CPU_ISR_Dispatch_disable)), %o3

  ! Store it to memory later to use the cycles
  ldd       [%o1 + L0_OFFSET], %l0        ! restore the local registers
  ldd       [%o1 + L2_OFFSET], %l2
  ldd       [%o1 + L4_OFFSET], %l4
  ldd       [%o1 + L6_OFFSET], %l6

  ! Now restore thread specific ISR dispatch prevention flag
  st        %o2,[%o3 + %lo(SYM(_CPU_ISR_Dispatch_disable))]

  ldd       [%o1 + I0_OFFSET], %i0        ! restore the output registers
  ldd       [%o1 + I2_OFFSET], %i2
  ldd       [%o1 + I4_OFFSET], %i4
  ldd       [%o1 + I6_FP_OFFSET], %i6

   /* restore the cache state (we may have entered here from an ISR) */
  set       XKY_SYSCALL_SPARC_RESTORE_CACHE_REGISTER, %o5
  ta        XKY_SYSCALL_SPARC_TRAP

  ldd       [%o1 + O2_OFFSET], %o2        ! restore the output registers
  ldd       [%o1 + O4_OFFSET], %o4
  ldd       [%o1 + O6_SP_OFFSET], %o6

  ! do o0/o1 last to avoid destroying heir context pointer
  ldd       [%o1 + O0_OFFSET], %o0        ! overwrite heir pointer

  jmp       %o7 + 8                       ! return
  nop                                     ! delay slot

        .align 4
        PUBLIC(_CPU_Context_restore)
SYM(_CPU_Context_restore):
  save      %sp, -CPU_MINIMUM_STACK_FRAME_SIZE, %sp   ! save stack
  ba        SYM(_CPU_Context_restore_heir)            ! jump to _CPU_Context_restore_heir
  mov       %i0, %o1                                  ! in the delay slot



        .align 4
        PUBLIC(_ISR_Handler)
SYM(_ISR_Handler):
  /**
   *  Fix the return address for synchronous traps.
   **/

  set       XKY_SYSCALL_SPARC_GET_PSR, %o5
  ta        XKY_SYSCALL_SPARC_TRAP
  mov       %o0, %l0

  andcc     %l3, SPARC_SYNCHRONOUS_TRAP_BIT_MASK, %g0
  ! Is this a synchronous trap?
  be,a      save_isf              ! No, then skip the adjustment
  nop                             ! DELAY
  mov       %l1, %l6              ! save trapped pc for debug info
  mov       %l2, %l1              ! do not return to the instruction
  add       %l2, 4, %l2           ! indicated

save_isf:
  /**
   *  It is not necessary to check if we are at an invalid windows because
   *  at the time that we get to this trap/interrupt handler, the PMK ISR
   *  routine already dealt with the invalid window
   */

  /**
   *  Save the state of the interrupted task -- especially the global
   *  registers -- in the Interrupt Stack Frame.  Note that the ISF
   *  includes a regular minimum stack frame which will be used if
   *  needed by register window overflow and underflow handlers.
   *
   *  REGISTERS SAME AS AT _ISR_Handler
   **/

  sub       %fp, CONTEXT_CONTROL_INTERRUPT_FRAME_SIZE, %sp

  ! make space for ISF
  std       %l0, [%sp + ISF_PSR_OFFSET]    ! save psr, PC
  st        %l2, [%sp + ISF_NPC_OFFSET]    ! save nPC
  st        %g1, [%sp + ISF_G1_OFFSET]     ! save g1
  std       %g2, [%sp + ISF_G2_OFFSET]     ! save g2, g3
  std       %g4, [%sp + ISF_G4_OFFSET]     ! save g4, g5
  std       %g6, [%sp + ISF_G6_OFFSET]     ! save g6, g7

  std       %i0, [%sp + ISF_I0_OFFSET]     ! save i0, i1
  std       %i2, [%sp + ISF_I2_OFFSET]     ! save i2, i3
  std       %i4, [%sp + ISF_I4_OFFSET]     ! save i4, i5
  std       %i6, [%sp + ISF_I6_FP_OFFSET]  ! save i6/fp, i7

  rd        %y, %g1
  st        %g1, [%sp + ISF_Y_OFFSET]      ! save y
  st        %l6, [%sp + ISF_TPC_OFFSET]    ! save real trapped pc

  mov       %sp, %g6                       ! 2nd arg to ISR Handler

  /**
   *  Increment ISR nest level and Thread dispatch disable level.
   *
   *  Register usage for this section:
   *
   *    l4 = _Thread_Dispatch_disable_level pointer
   *    l5 = _ISR_Nest_level pointer
   *    l6 = _Thread_Dispatch_disable_level value
   *    l7 = _ISR_Nest_level value
   *
   *  NOTE: It is assumed that l4 - l7 will be preserved until the ISR
   *        nest and thread dispatch disable levels are unnested.
   **/
  sethi     %hi(SYM(_Thread_Dispatch_disable_level)), %l4
  ld        [%l4 + %lo(SYM(_Thread_Dispatch_disable_level))], %l6
  sethi     %hi(SYM(_ISR_Nest_level)), %l5
  ld        [%l5 + %lo(SYM(_ISR_Nest_level))], %l7

  add       %l6, 1, %l6
  st        %l6, [%l4 + %lo(SYM(_Thread_Dispatch_disable_level))]

  add       %l7, 1, %l7
  st        %l7, [%l5 + %lo(SYM(_ISR_Nest_level))]

  /**
   *  If ISR nest level was zero (now 1), then switch stack.
   **/
  mov       %sp, %fp
  subcc     %l7, 1, %l7             ! outermost interrupt handler?
  bnz       dont_switch_stacks      ! No, then do not switch stacks

  sethi     %hi(SYM(_CPU_Interrupt_stack_high)), %g4
  ld        [%g4 + %lo(SYM(_CPU_Interrupt_stack_high))], %sp

dont_switch_stacks:

  /**
   *  Make sure we have a place on the stack for the window overflow
   *  trap handler to write into.  At this point it is safe to
   *  enable traps again.
   **/
  sub       %sp, CPU_MINIMUM_STACK_FRAME_SIZE, %sp

  /**
   *  Check if we have an external interrupt (trap 0x11 - 0x1f). If so,
   *  set the PIL in the %psr to mask off interrupts with lower priority.
   *  The original %psr in %l0 is not modified since it will be restored
   *  when the interrupt handler returns.
   **/
  mov       %l0, %g5
  and       %l3, 0x0ff, %g4


  subcc     %g4, 0x11, %g0
  bl        dont_fix_pil
  subcc     %g4, 0x1f, %g0
  bg        dont_fix_pil
  sll       %g4, 8, %g4
  and       %g4, SPARC_PSR_PIL_MASK, %g4
  andn      %l0, SPARC_PSR_PIL_MASK, %g5
  ba        pil_fixed
  or        %g4, %g5, %g5

dont_fix_pil:
  or        %g5, SPARC_PSR_PIL_MASK, %g5
pil_fixed:
  or        %g5, SPARC_PSR_ET_MASK, %o0 //parameters
  set       XKY_SYSCALL_SPARC_SET_PSR, %o5 //id trap
  ta        XKY_SYSCALL_SPARC_TRAP      ! **** ENABLE TRAPS ****
dont_fix_pil2:

  /**
   *  Vector to user's handler.
   *
   *  NOTE: TBR may no longer have vector number in it since
   *        we just enabled traps.  It is definitely in l3.
   **/
  mov       %g6, %o1
  sethi     %hi(SYM(_ISR_Vector_table)), %g4
  ld        [%g4+%lo(SYM(_ISR_Vector_table))], %g4
  tst       %g4                    ! check if the pointer is valid
  bz        skip_user_handler      ! no? then skip user handler
  and       %l3, 0xFF, %g5         ! remove synchronous trap indicator
  sll       %g5, 2, %g5            ! g5 = offset into table
  ld        [%g4 + %g5], %g4       ! g4 = _ISR_Vector_table[ vector ]

  tst       %g4                    ! check if the pointer is valid
  bz        skip_user_handler      ! no? then skip user handler

  ! o1 = 2nd arg = address of the ISF
  !   WAS LOADED WHEN ISF WAS SAVED!!!
  mov       %l3, %o0               ! o0 = 1st arg = vector number
  call      %g4, 0
  nop                             ! delay slot

skip_user_handler:

  /**
   *  Redisable traps so we can finish up the interrupt processing.
   *  This is a VERY conservative place to do this.
   *
   *  NOTE: %l0 has the PSR which was in place when we took the trap.
   **/

  mov       %l0, %o1
  set       XKY_SYSCALL_SPARC_SET_PSR, %o5
  ta        XKY_SYSCALL_SPARC_TRAP  !  **** DISABLE TRAPS ****

  /**
   *  Decrement ISR nest level and Thread dispatch disable level.
   *
   *  Register usage for this section:
   *
   *    l4 = _Thread_Dispatch_disable_level pointer
   *    l5 = _ISR_Nest_level pointer
   *    l6 = _Thread_Dispatch_disable_level value
   *    l7 = _ISR_Nest_level value
   **/

  sub       %l6, 1, %l6
  st        %l6, [%l4 + %lo(SYM(_Thread_Dispatch_disable_level))]

  st        %l7, [%l5 + %lo(SYM(_ISR_Nest_level))]

  /**
   *  If dispatching is disabled (includes nested interrupt case),
   *  then do a "simple" exit.
   **/

  orcc      %l6, %g0, %g0   ! Is dispatching disabled?
  bnz       simple_return   ! Yes, then do a "simple" exit
  ! NOTE: Use the delay slot
  sethi     %hi(SYM(_CPU_ISR_Dispatch_disable)), %l6

  ! Are we dispatching from a previous ISR in the interrupted thread?
  ld        [%l6 + %lo(SYM(_CPU_ISR_Dispatch_disable))], %l7
  orcc      %l7, %g0, %g0   ! Is this thread already doing an ISR?
  bnz       simple_return   ! Yes, then do a "simple" exit
  ! NOTE: Use the delay slot
  sethi     %hi(SYM(_Context_Switch_necessary)), %l4


  /**
   *  If a context switch is necessary, then do fudge stack to
   *  return to the interrupt dispatcher.
   **/

  ld        [%l4 + %lo(SYM(_Context_Switch_necessary))], %l5

  orcc      %l5, %g0, %g0       ! Is thread switch necessary?
  bz        simple_return       ! no, go to simple_return.
                                ! yes, then invoke the dispatcher
  nop                           ! delay slot


        PUBLIC(_ISR_Dispatch)
SYM(_ISR_Dispatch):
  ! Set ISR dispatch nesting prevention flag
  mov       1,%l6
  sethi     %hi(SYM(_CPU_ISR_Dispatch_disable)), %l5
  st        %l6,[%l5 + %lo(SYM(_CPU_ISR_Dispatch_disable))]

  /**
   *  The following subtract should get us back on the interrupted
   *  tasks stack and add enough room to invoke the dispatcher.
   *  When we enable traps, we are mostly back in the context
   *  of the task and subsequent interrupts can operate normally.
   **/

  sub       %fp, CPU_MINIMUM_STACK_FRAME_SIZE, %sp

  or        %l0, SPARC_PSR_ET_MASK, %l7    ! l7 = PSR with ET=1
  mov       %l7, %o0
  set       XKY_SYSCALL_SPARC_SET_PSR, %o5
  ta        XKY_SYSCALL_SPARC_TRAP         !  **** ENABLE TRAPS ****

isr_dispatch:
  call      SYM(_Thread_Dispatch), 0
  nop

  /**
   *  We invoked _Thread_Dispatch in a state similar to the interrupted
   *  task.  In order to safely be able to tinker with the register
   *  windows and get the task back to its pre-interrupt state,
   *  we need to disable interrupts disabled so we can safely tinker
   *  with the register windowing.  In particular, the CWP in the PSR
   *  is fragile during this period. (See PR578.)
   **/
  set       XKY_SYSCALL_SPARC_DISABLE_INTERRUPTS, %o5
  ta        XKY_SYSCALL_SPARC_TRAP       !  **** DISABLE INTERRUPTS ****

  /**
   *  While we had ISR dispatching disabled in this thread,
   *  did we miss anything.  If so, then we need to do another
   *  _Thread_Dispatch before leaving this ISR Dispatch context.
   **/

  sethi     %hi(SYM(_Context_Switch_necessary)), %l4
  ld        [%l4 + %lo(SYM(_Context_Switch_necessary))], %l5

  orcc      %l5, %g0, %g0       ! Is thread switch necessary?
  bz        allow_nest_again    ! No, then clear out and return
  nop                           ! delay slot
                                ! Yes, then invoke the dispatcher

dispatchAgain:
  set       XKY_SYSCALL_SPARC_ENABLE_INTERRUPTS, %o5
  ta        XKY_SYSCALL_SPARC_TRAP       !  **** ENABLE INTERRUPTS ****
  ba        isr_dispatch
  nop

allow_nest_again:

  ! Zero out ISR stack nesting prevention flag
  sethi    %hi(SYM(_CPU_ISR_Dispatch_disable)), %l5
  st       %g0,[%l5 + %lo(SYM(_CPU_ISR_Dispatch_disable))]

  /**
   *  The CWP in place at this point may be different from
   *  that which was in effect at the beginning of the ISR if we
   *  have been context switched between the beginning of this invocation
   *  of _ISR_Handler and this point.  Thus the CWP and WIM should
   *  not be changed back to their values at ISR entry time.  Any
   *  changes to the PSR must preserve the CWP.
   **/

simple_return:
  ld        [%fp + ISF_Y_OFFSET], %l5      ! restore y
  wr        %l5, 0, %y

  ldd       [%fp + ISF_PSR_OFFSET], %l0    ! restore psr, PC
  ld        [%fp + ISF_NPC_OFFSET], %l2    ! restore nPC
  andn      %l0, SPARC_PSR_ET_MASK, %l0

  /**
   *  Restore tasks global and out registers
   **/

  mov       %fp, %o2

  ld        [%fp + ISF_G1_OFFSET], %g1    ! restore g1
  ldd       [%fp + ISF_G2_OFFSET], %g2    ! restore g2, g3
  ldd       [%fp + ISF_G4_OFFSET], %g4    ! restore g4, g5
  ldd       [%fp + ISF_G6_OFFSET], %g6    ! restore g6, g7

  ldd       [%fp + ISF_I0_OFFSET], %i0    ! restore i0, i1
  ldd       [%fp + ISF_I2_OFFSET], %i2    ! restore i2, i3
  ldd       [%fp + ISF_I4_OFFSET], %i4    ! restore i4, i5
  ldd       [%fp + ISF_I6_FP_OFFSET], %i6 ! restore i6/fp, i7

  mov       %l0, %o0
  set       XKY_SYSCALL_SPARC_SET_PSR, %o5
  ta        XKY_SYSCALL_SPARC_TRAP         !  **** DISABLE TRAPS ****

  /* restore the cache state  */
  set       XKY_SYSCALL_SPARC_RESTORE_CACHE_REGISTER, %o5
  ta        XKY_SYSCALL_SPARC_TRAP

  /**
   *  Return from trap
   */
  mov       %l1, %o0                ! transfer control and
  mov       %l2, %o1                ! go back to tasks window
  or        %g0, XKY_SYSCALL_SPARC_RETT, %o5
  ta        XKY_SYSCALL_SPARC_TRAP

/* end of file */

/**
 * \endcond
 */

/**
 *  @}
 */

/**
 *  @}
 */
