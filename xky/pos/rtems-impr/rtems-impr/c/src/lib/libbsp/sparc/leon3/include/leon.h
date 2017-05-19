/**
 *  @file
 *  leon.h
 *
 *  @brief LEON3 BSP data types and macros.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5958    | 12/01/2010  | mcoutinho    | IPR 2532
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @defgroup SPARC_LEON3_BSP SPARC LEON3 BSP
 *
 *  @brief The LEON3 BSP contains the specific code related to the LEON3 BSP
 *  @{
 */

#ifndef _INCLUDE_LEON_h
#define _INCLUDE_LEON_h

#include <rtems/score/sparc.h>
#include <amba.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define LEON_INTERRUPT_EXTERNAL_1 5

#ifndef ASM

   /**
    *  @brief trap types for on-chip peripherals
    *
    *  Source: Table 8 - Interrupt Trap Type and Default Priority Assignments
    *
    *  @note The priority level for each source corresponds to the least
    *        significant nibble of the trap type.
    */
#define LEON_TRAP_TYPE( _source ) SPARC_ASYNCHRONOUS_TRAP((_source) + 0x10)

   /**
    * @brief determine the leon3 trap source number
    *
    * @param[in] _trap the trap number
    **/
#define LEON_TRAP_SOURCE( _trap ) ((_trap) - 0x10)

   /**
    * @brief determine the leon3 interrupt trap type
    *
    * @param[in] _trap the trap number
    **/
#define LEON_INT_TRAP( _trap ) \
  ( (_trap) >= 0x11 && \
    (_trap) <= 0x1F )

   /**
    * @brief leon3 uart registers map
    **/
   typedef struct
   {
      volatile unsigned int data;
      volatile unsigned int status;
      volatile unsigned int ctrl;
      volatile unsigned int scaler;
   } LEON3_UART_Regs_Map;

   /**
    * @brief leon3 registers per timer
    **/
   typedef struct
   {
      volatile unsigned int value;
      volatile unsigned int reload;
      volatile unsigned int conf;
      volatile unsigned int notused;
   } LEON3_Timer_SubType;

   /**
    * @brief leon3 timer register map
    **/
   typedef struct
   {
      volatile unsigned int scaler_value; /* common timer registers */
      volatile unsigned int scaler_reload;
      volatile unsigned int status;
      volatile unsigned int notused;
      LEON3_Timer_SubType timer[8];
   } LEON3_Timer_Regs_Map;

   /**
    * @brief leon3 input/output register map
    **/
   typedef struct
   {
      volatile unsigned int iodata;
      volatile unsigned int ioout;
      volatile unsigned int iodir;
      volatile unsigned int irqmask;
      volatile unsigned int irqpol;
      volatile unsigned int irqedge;
   } LEON3_IOPORT_Regs_Map;

#endif

   /**
    *  @brief define the bits in Memory Configuration Register 1.
    */
#define LEON_MEMORY_CONFIGURATION_PROM_SIZE_MASK  0x0003C000

   /**
    *  @brief define the bits in Memory Configuration Register 1.
    */
#define LEON_MEMORY_CONFIGURATION_RAM_SIZE_MASK  0x00001E00

   /**
    *  @brief define the bits in the Timer Control Register.
    */
#define LEON_REG_TIMER_CONTROL_EN    0x00000001  /* 1 = enable counting */
   /* 0 = hold scalar and counter */
#define LEON_REG_TIMER_CONTROL_RL    0x00000002  /* 1 = reload at 0 */
   /* 0 = stop at 0 */
#define LEON_REG_TIMER_CONTROL_LD    0x00000004  /* 1 = load counter */
   /* 0 = no function */

   /*
    *  The following defines the bits in the UART Control Registers.
    */



#define LEON_REG_UART_CONTROL_RTD  0x000000FF /* RX/TX data */ 

   /*
    *  The following defines the bits in the LEON UART Status Registers.
    */

#define LEON_REG_UART_STATUS_DR   0x00000001 /* Data Ready */
#define LEON_REG_UART_STATUS_TSE  0x00000002 /* TX Send Register Empty */
#define LEON_REG_UART_STATUS_THE  0x00000004 /* TX Hold Register Empty */
#define LEON_REG_UART_STATUS_BR   0x00000008 /* Break Error */
#define LEON_REG_UART_STATUS_OE   0x00000010 /* RX Overrun Error */
#define LEON_REG_UART_STATUS_PE   0x00000020 /* RX Parity Error */
#define LEON_REG_UART_STATUS_FE   0x00000040 /* RX Framing Error */
#define LEON_REG_UART_STATUS_ERR  0x00000078 /* Error Mask */


   /*
    *  The following defines the bits in the LEON UART Status Registers.
    */

#define LEON_REG_UART_CTRL_RE     0x00000001 /* Receiver enable */
#define LEON_REG_UART_CTRL_TE     0x00000002 /* Transmitter enable */
#define LEON_REG_UART_CTRL_RI     0x00000004 /* Receiver interrupt enable */
#define LEON_REG_UART_CTRL_TI     0x00000008 /* Transmitter interrupt enable */
#define LEON_REG_UART_CTRL_PS     0x00000010 /* Parity select */
#define LEON_REG_UART_CTRL_PE     0x00000020 /* Parity enable */
#define LEON_REG_UART_CTRL_FL     0x00000040 /* Flow control enable */
#define LEON_REG_UART_CTRL_LB     0x00000080 /* Loop Back enable */

   /**
    * @brief LEON3 Interrupt Controller register
    */
   extern volatile LEON3_IrqCtrl_Regs_Map *LEON3_IrqCtrl_Regs;

   /**
    * @brief LEON3 GP Timer register
    */
   extern volatile LEON3_Timer_Regs_Map *LEON3_Timer_Regs;

   /**
    * @brief LEON3 UARTs register
    **/
   extern volatile LEON3_UART_Regs_Map *LEON3_Console_Uart[LEON3_APBUARTS];

#if defined(RTEMS_MULTIPROCESSING)
   /**
    * @brief LEON3 current CPU index
    **/
   extern int LEON3_Cpu_Index;
#endif

   /* Macros used for manipulating bits in LEON3 GP Timer Control Register */

#define LEON3_GPTIMER_EN 1
#define LEON3_GPTIMER_RL 2
#define LEON3_GPTIMER_LD 4
#define LEON3_GPTIMER_IRQEN 8

#define LEON3_MP_IRQ    14        /* Irq used by shared memory driver */

#ifndef ASM

   /*
    *  Macros to manipulate the Interrupt Clear, Interrupt Force, Interrupt Mask,
    *  and the Interrupt Pending Registers.
    *
    *  NOTE: For operations which are not atomic, this code disables interrupts
    *        to guarantee there are no intervening accesses to the same register.
    *        The operations which read the register, modify the value and then
    *        store the result back are vulnerable.
    */

   /**
    * @brief clear the leon3 interrupt source
    **/
#define LEON_Clear_interrupt( _source ) \
  do { \
    LEON3_IrqCtrl_Regs->iclear = (1 << (_source)); \
  } while (0)

#define LEON_Force_interrupt( _source ) \
  do { \
    LEON3_IrqCtrl_Regs->iforce = (1 << (_source)); \
  } while (0)

#define LEON_Is_interrupt_pending( _source ) \
  (LEON3_IrqCtrl_Regs.ipend & (1 << (_source)))

#if defined(RTEMS_MULTIPROCESSING)

#define LEON_Is_interrupt_masked( _source ) \
  do {\
     (LEON3_IrqCtrl_Regs.mask[LEON3_Cpu_Index] & (1 << (_source))); \
   } while (0)


#define LEON_Mask_interrupt( _source ) \
  do { \
    uint32_t _level; \
    _level = xky_sparc_disable_interrupts(); \
     LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index]  &= ~(1 << (_source)); \
    xky_sparc_enable_interrupts( _level ); \
  } while (0)

   /**
    * @brief unmask a leon3 interrupt
    **/
#define LEON_Unmask_interrupt( _source ) \
  do { \
    uint32_t _level; \
    _level = xky_sparc_disable_interrupts(); \
    LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index]  |= (1 << (_source)); \
    xky_sparc_enable_interrupts( _level ); \
  } while (0)

#define LEON_Disable_interrupt( _source, _previous ) \
  do { \
    uint32_t _level; \
    uint32_t _mask = 1 << (_source); \
    _level = xky_sparc_disable_interrupts(); \
     (_previous) = LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index]; \
     LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] = _previous & ~_mask; \
    xky_sparc_enable_interrupts( _level ); \
    (_previous) &= _mask; \
  } while (0)

#define LEON_Restore_interrupt( _source, _previous ) \
  do { \
    uint32_t _level; \
    uint32_t _mask = 1 << (_source); \
    _level = xky_sparc_disable_interrupts(); \
      LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] = \
        (LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] & ~_mask) | (_previous); \
    xky_sparc_enable_interrupts( _level ); \
  } while (0)

#else

#define LEON_Is_interrupt_masked( _source ) \
  do {\
     (LEON3_IrqCtrl_Regs.mask[0] & (1 << (_source))); \
   } while (0)


#define LEON_Mask_interrupt( _source ) \
  do { \
    uint32_t _level; \
    _level = xky_sparc_disable_interrupts(); \
     LEON3_IrqCtrl_Regs->mask[0]  &= ~(1 << (_source)); \
    xky_sparc_enable_interrupts( _level ); \
  } while (0)

   /**
    * @brief unmask a leon3 interrupt
    **/
#define LEON_Unmask_interrupt( _source ) \
  do { \
    uint32_t _level; \
    _level = xky_sparc_disable_interrupts(); \
    LEON3_IrqCtrl_Regs->mask[0]  |= (1 << (_source)); \
    xky_sparc_enable_interrupts( _level ); \
  } while (0)

#define LEON_Disable_interrupt( _source, _previous ) \
  do { \
    uint32_t _level; \
    uint32_t _mask = 1 << (_source); \
    _level = xky_sparc_disable_interrupts(); \
     (_previous) = LEON3_IrqCtrl_Regs->mask[0]; \
     LEON3_IrqCtrl_Regs->mask[0] = _previous & ~_mask; \
    xky_sparc_enable_interrupts( _level ); \
    (_previous) &= _mask; \
  } while (0)

#define LEON_Restore_interrupt( _source, _previous ) \
  do { \
    uint32_t _level; \
    uint32_t _mask = 1 << (_source); \
    _level = xky_sparc_disable_interrupts(); \
      LEON3_IrqCtrl_Regs->mask[0] = \
        (LEON3_IrqCtrl_Regs->mask[0] & ~_mask) | (_previous); \
    xky_sparc_enable_interrupts( _level ); \
  } while (0)

#endif

   /*
    *  Each timer control register is organized as follows:
    *
    *    D0 - Enable
    *          1 = enable counting
    *          0 = hold scaler and counter
    *
    *    D1 - Counter Reload
    *          1 = reload counter at zero and restart
    *          0 = stop counter at zero
    *
    *    D2 - Counter Load
    *          1 = load counter with preset value
    *          0 = no function
    *
    */

#define LEON_REG_TIMER_COUNTER_RELOAD_AT_ZERO     0x00000002
#define LEON_REG_TIMER_COUNTER_STOP_AT_ZERO       0x00000000

#define LEON_REG_TIMER_COUNTER_LOAD_COUNTER       0x00000004

#define LEON_REG_TIMER_COUNTER_ENABLE_COUNTING    0x00000001
#define LEON_REG_TIMER_COUNTER_DISABLE_COUNTING   0x00000000

#define LEON_REG_TIMER_COUNTER_RELOAD_MASK        0x00000002
#define LEON_REG_TIMER_COUNTER_ENABLE_MASK        0x00000001

#define LEON_REG_TIMER_COUNTER_DEFINED_MASK       0x00000003
#define LEON_REG_TIMER_COUNTER_CURRENT_MODE_MASK  0x00000003

#endif /* !ASM */

#ifdef __cplusplus
}
#endif

#endif /* !_INCLUDE_LEON_h */
/* end of include file */

/**
 *  @}
 */
