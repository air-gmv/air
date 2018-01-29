/*
 * Copyright (c) 2012, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/config.h>
#include <rtems/counter.h>

#include <bsp.h>
#include <bsp/vectors.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>

LINKER_SYMBOL(bsp_exc_vector_base);

/*
 * Configuration parameter for clock driver.  The Trace32 PowerPC simulator has
 * an odd decrementer frequency.  The time base frequency is one tick per
 * instruction.  The decrementer frequency is one tick per ten instructions.
 * The clock driver assumes that the time base and decrementer frequencies are
 * equal.  For now we simulate processor that issues 10000000 instructions per
 * second.
 */
uint32_t bsp_time_base_frequency = 10000000;

void BSP_panic(char *s)
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  (void) level;

  printk("%s PANIC %s\n", rtems_get_version_string(), s);

  while (1) {
    /* Do nothing */
  }
}

void _BSP_Fatal_error(unsigned n)
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  (void) level;

  printk("%s PANIC ERROR %u\n", rtems_get_version_string(), n);

  while (1) {
    /* Do nothing */
  }
}

#define MTIVPR(base) \
  __asm__ volatile ("mtivpr %0" : : "r" (base))

#define VECTOR_TABLE_ENTRY_SIZE 16

#define MTIVOR(vec, offset) \
  do { \
    __asm__ volatile ("mtspr " RTEMS_XSTRING(vec) ", %0" : : "r" (offset)); \
    offset += VECTOR_TABLE_ENTRY_SIZE; \
  } while (0)

static void t32mppc_initialize_exceptions(void *interrupt_stack_begin)
{
  uintptr_t addr;

  ppc_exc_initialize_interrupt_stack(
    (uintptr_t) interrupt_stack_begin,
    rtems_configuration_get_interrupt_stack_size()
  );

  addr = (uintptr_t) bsp_exc_vector_base;
  MTIVPR(addr);
  MTIVOR(BOOKE_IVOR0,  addr);
  MTIVOR(BOOKE_IVOR1,  addr);
  MTIVOR(BOOKE_IVOR2,  addr);
  MTIVOR(BOOKE_IVOR3,  addr);
  MTIVOR(BOOKE_IVOR4,  addr);
  MTIVOR(BOOKE_IVOR5,  addr);
  MTIVOR(BOOKE_IVOR6,  addr);
  MTIVOR(BOOKE_IVOR7,  addr);
  MTIVOR(BOOKE_IVOR8,  addr);
  MTIVOR(BOOKE_IVOR9,  addr);
  MTIVOR(BOOKE_IVOR10, addr);
  MTIVOR(BOOKE_IVOR11, addr);
  MTIVOR(BOOKE_IVOR12, addr);
  MTIVOR(BOOKE_IVOR13, addr);
  MTIVOR(BOOKE_IVOR14, addr);
  MTIVOR(BOOKE_IVOR15, addr);
  MTIVOR(BOOKE_IVOR32, addr);
  MTIVOR(BOOKE_IVOR33, addr);
  MTIVOR(BOOKE_IVOR34, addr);
  MTIVOR(BOOKE_IVOR35, addr);
}

void bsp_start(void)
{
  get_ppc_cpu_type();
  get_ppc_cpu_revision();

  rtems_counter_initialize_converter(bsp_time_base_frequency);

  t32mppc_initialize_exceptions(bsp_section_work_begin);
  bsp_interrupt_initialize();
}
