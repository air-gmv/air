/**
 * @file
 *
 * @ingroup powerpc_shared
 *
 * @brief Source file for a clock driver.
 */

/*
 * Copyright (c) 2008, 2009
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/clockdrv.h>

#include <libcpu/powerpc-utility.h>
#include <bsp/vectors.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

/*
 * This variable must be defined in the BSP and valid before clock driver
 * initialization.  The clicks refer to the decrementer and time base.
 */
extern uint32_t bsp_clicks_per_usec;

#define PPC_CLOCK_DECREMENTER_MAX UINT32_MAX

volatile uint32_t Clock_driver_ticks = 0;

rtems_device_major_number rtems_clock_major = UINT32_MAX;

rtems_device_minor_number rtems_clock_minor = UINT32_MAX;

static uint32_t ppc_clock_decrementer_value = PPC_CLOCK_DECREMENTER_MAX;

static uint32_t ppc_clock_next_time_base = 0;

static void ppc_clock_no_tick(void)
{
	/* Do nothing */
}

static void (*ppc_clock_tick)(void) = ppc_clock_no_tick;

static int ppc_clock_exception_handler( BSP_Exception_frame *frame, unsigned number)
{
	uint32_t delta = ppc_clock_decrementer_value;
	uint32_t next = ppc_clock_next_time_base;
	uint32_t dec = 0;
	uint32_t now = 0;
	uint32_t msr = 0;

	do {
		/* Increment clock ticks */
		Clock_driver_ticks += 1;

		/* Enable external exceptions */
		msr = ppc_external_exceptions_enable();

		/* Call clock ticker  */
		ppc_clock_tick();

		/* Restore machine state */
		ppc_external_exceptions_disable( msr);

		/* Next time base */
		next += delta;

		/* Current time */
		now = ppc_time_base();

		/* New decrementer value */
		dec = next - now;
	} while (dec > delta);

	/* Set decrementer */
	ppc_set_decrementer_register( dec);

	/* Expected next time base */
	ppc_clock_next_time_base = next;

	return 0;
}


static int ppc_clock_exception_handler_booke( BSP_Exception_frame *frame, unsigned number)
{
	uint32_t msr;

	/* Acknowledge decrementer request */
	PPC_SET_SPECIAL_PURPOSE_REGISTER( BOOKE_TSR, BOOKE_TSR_DIS);

	/* Increment clock ticks */
	Clock_driver_ticks += 1;

	/* Enable external exceptions */
	msr = ppc_external_exceptions_enable();

	/* Call clock ticker  */
	ppc_clock_tick();

	/* Restore machine state */
	ppc_external_exceptions_disable( msr);

	return 0;
}

static int ppc_clock_exception_handler_e300( BSP_Exception_frame *frame, unsigned number)
{
	uint32_t msr;

	/* Increment clock ticks */
	Clock_driver_ticks += 1;

	/* Enable external exceptions */
	msr = ppc_external_exceptions_enable();

	/* Call clock ticker  */
	ppc_clock_tick();

	/* Restore machine state */
	ppc_external_exceptions_disable( msr);

	return 0;
}

static uint32_t ppc_clock_nanoseconds_since_last_tick(void)
{
	return ((ppc_clock_decrementer_value - ppc_decrementer_register()) * 1000) / bsp_clicks_per_usec;
}

void Clock_exit(void)
{
	/* Set the decrementer to the maximum value */
	ppc_set_decrementer_register( PPC_CLOCK_DECREMENTER_MAX);

	/* Use default clock handler */
	ppc_clock_tick = ppc_clock_no_tick;
}

rtems_device_driver Clock_initialize( rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	/* Current CPU type */
	ppc_cpu_id_t cpu_type = get_ppc_cpu_type();

	/* Make major/minor available to others such as shared memory driver */
	rtems_clock_major = major;
	rtems_clock_minor = minor;

	/*
	 * Set default ticker.
	 *
	 * The function rtems_clock_tick() returns a status code.  This value
	 * will be discarded since the RTEMS documentation claims that it is
	 * always successful.
	 */
	ppc_clock_tick = (void (*)(void)) rtems_clock_tick;

	/* Set the decrementer to the maximum value */
	ppc_set_decrementer_register( PPC_CLOCK_DECREMENTER_MAX);

	/* Decrementer value */
	ppc_clock_decrementer_value = bsp_clicks_per_usec * rtems_configuration_get_microseconds_per_tick() - 1;

	/* Check decrementer value */
	if (ppc_clock_decrementer_value == 0) {
		ppc_clock_decrementer_value = PPC_CLOCK_DECREMENTER_MAX;
		RTEMS_SYSLOG_ERROR( "decrementer value would be zero, will be set to maximum value instead\n");
	}

	/* Set the nanoseconds since last tick handler */
	rtems_clock_set_nanoseconds_extension( ppc_clock_nanoseconds_since_last_tick);

	if (ppc_cpu_is_bookE()) {
		/* Set decrementer auto-reload value */
		PPC_SET_SPECIAL_PURPOSE_REGISTER( BOOKE_DECAR, ppc_clock_decrementer_value);

		/* Install exception handler */
		ppc_exc_set_handler( ASM_BOOKE_DEC_VECTOR, ppc_clock_exception_handler_booke);

		/* Enable decrementer and auto-reload */
		PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS( BOOKE_TCR, BOOKE_TCR_DIE | BOOKE_TCR_ARE);
	} else if (cpu_type == PPC_e300c2 || cpu_type == PPC_e300c3) {
		/* TODO: Not tested for e300c2 */

		/* Enable auto-reload */
		PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS( HID0, 0x00000040);

		/* Install exception handler */
		ppc_exc_set_handler( ASM_DEC_VECTOR, ppc_clock_exception_handler_e300);
	} else {
		/* Here the decrementer value is actually the interval */
		++ppc_clock_decrementer_value;

		/* Initialize next time base */
		ppc_clock_next_time_base = ppc_time_base() + ppc_clock_decrementer_value;

		/* Install exception handler */
		ppc_exc_set_handler( ASM_DEC_VECTOR, ppc_clock_exception_handler);
	}

	/* Set the decrementer value */
	ppc_set_decrementer_register( ppc_clock_decrementer_value);

	return RTEMS_SUCCESSFUL;
}
